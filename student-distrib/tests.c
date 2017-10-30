#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt_init.h"
#include "keyboard.h"
#include "rtc.h"
#include "filesystem.h"

#define PASS     	1
#define FAIL 		0
#define BUFFER_SIZE	128


/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}
	return result;
}

/* Checkpoint 1 Tests */

/* div_zero_test
*		Description: tezts the divide by zero exception handler being called and such
*		Author: Jonathan
*		Input: None
*		Output: None
*		Returns: always FAIL because you shouldn't reach return
*		Side effects: tries to do '5 / 0' which should result in reaching the divide by zero excpetion handler
*/
/*
int div_zero_test(){
	TEST_HEADER;
	int i;
	i=5/0;
	return FAIL;
}
*/
int paging_test(){
	TEST_HEADER;


	//int* kernel_cs_test = 0x0400000;
	//int* vid_mem_test = 0xB8000;
	int null_test = *(int *)0;
	null_test += null_test;
	//printf("%d\n", *(kernel_cs_test));
	//printf("%d\n", *(vid_mem_test));
	//printf("%d\n", *(null_test));
	return 0;
/*

	int* kernel_cs_test = 0x0400019B;
	int* kernel_cs_test_2 = 0x04000000;
	int* vid_mem_test = 0xB811B;
	int* vid_mem_test_2 = 0xB8000;
	int* null_test = NULL;

	printf("%d\n", *(kernel_cs_test));
	printf("%d\n", *(kernel_cs_test_2));
	printf("%d\n", *(vid_mem_test));
	printf("%d\n", *(vid_mem_test_2));
	printf("%d\n", *(null_test));
 	return PASS;
*/
 }

/* Checkpoint 2 tests */

/*
*	terminal_driver_test
*		Author: Sam
*		Description: Tests the terminal drivers
*		Input: none
*		Output: Test status to screen
*		Return: Pass/Fail (1/0)
*/

int terminal_driver_test(){
	int test_open;
	int test_read;
	int test_write;
	int test_close;

	char buffer[BUFFER_SIZE];

	TEST_HEADER;
	test_open = terminal_open((uint8_t*)1);
	if(test_open != 0){
		return FAIL;
	}
	printf("What's your name?\n");
	test_read = terminal_read(0, buffer, BUFFER_SIZE);
	if(test_read < 0){
		return FAIL;
	}
	printf("Hello, ");
	test_write = terminal_write(0, buffer, test_read);
	printf("\n");
	if(test_write != 0){
		return FAIL;
	}

	

	test_close = terminal_close(1);
	if(test_close != 0){
		return FAIL;
	}



	return PASS;

}



/*
*	rtc_helper
*		Author: Jonathan
*		Description: helper for the rtc_test function. 
*				   		Uses rtc_read to print a 1 after a rtc_iterrupt for a 
*				 		specified number of rtc_interrupts.
*		Inputs: integer n number of interrupts to print after
*		Outputs: none
*		Returns: nothing
*		Side effects: Prints to screen after rtc interrupts
*/
void rtc_helper(int n){
	while(n>0){
		rtc_read(0, NULL, 0); //wait for interrupt
		char one[1];
		one[0] = '1';
		terminal_write(0, one, 1);
		n--;	//update counters
	}
	printf("\n");
	return;
}
/* 
*	rtc_test
*		Author: Jonathan
*		Description: Tests, the rtc functions. This verifies that the return value for each function
*						(rtc read/write/open/close) is correct (fails on bad input - succeeds on good)
*						The change in frequency is shown to the user by printing '1' on each rtc_read
*`						Printing uses the helper function rtc_helper
*		Inputs: none
*		Outputs: none
*		Returns: PASS or FAIL (1 or 0)
*		Side Effects: Functionality Validation
*/
int rtc_test(){
	printf("\n");
	TEST_HEADER;
	printf("This test will print the character '1' when an RTC interrupt occurs \n(as determined using rtc_read)\nStarting with rtc_open which will set it to 2Hz\n");
	
	//Initialize to 2Hz, check ret value, and print 20 1's (10s)
	int ret;  //to hold return vals
	ret = rtc_open(NULL);
	if(ret!=0){
		return FAIL;
	}
	rtc_helper(20);  
	clear();
	int freq = 2;
	//Call RTC write for each possible rate and print for ~4s
	do{
		freq = freq * 2; //increase to next power of two
		printf("Calling RTC write with rate of %dHz\n", freq);
		ret = rtc_write(0, NULL, freq);
		if(ret==-1){	//make sure it doesnt fail
			return FAIL;
		}
		rtc_helper(4*freq);  //print for 4s
		clear();
	}while(freq<1024);
	
	printf("Putting RTC write freq back to 2Hz\n");
	ret = rtc_write(0, NULL, 2);
	if(ret==-1){	//make sure it doesnt fail
		return FAIL;
	}
	rtc_helper(8);  //print for 4s

	//Check bounds on RTC Write function
	printf("\nCalling RTC write with invalid rates\n");
	freq = -499;
	do{
		freq = freq + 250; //increase to next power of two
		printf("Calling RTC write with rate of %dHz\n", freq);
		ret = rtc_write(0, NULL, freq);
		if(ret==0){	//make sure it doesnt succeed
			return FAIL;
		}
	}while(freq<2500);
	printf("Calling RTC write with rate of NULLHz\n");
	ret = rtc_write(0, NULL, NULL);
	if(ret==0){	//make sure it doesnt succeed
		return FAIL;
	}
	//input check the frequency with NULL (only the 3rd input is used by the function)
	ret = rtc_write(0, NULL, NULL);
	if(ret==0){
		return FAIL;
	}
	//show that it was not changed... print 10 1's (7.5s)
	rtc_helper(15);
	printf("Still running at 2Hz\n");

	//verify that rtc close returns 0 
	ret = rtc_close(0);
	if(ret!=0){
		return FAIL;
	}
	return PASS;
} 



/* file_syscalls_test() 
*		Description: tests the file system calls in the following order:
					 - file_open
					 - file_read
					 - file_write
					 - file_close
*		Author: Austin
*		Input: None
*		Output: PASS for success, FAIL for failure
*		Side effects: Clears terminal, prints to terminal
*/
int file_syscalls_test() {
	clear();
	TEST_HEADER;


	char* name_1;
	char* name_2;
	char* name_3;
	uint8_t buf[BUF_SIZE];
	uint8_t fd;
	int32_t nbytes;
	nbytes = BUF_SIZE;

	name_1 = "cat";
	name_2 = "notafile";
	name_3 = ".";
	fd = 0;

	/* Test file_open */
	if(file_open((uint8_t *)name_1) == -1){
		return FAIL;
	}
	if(file_open((uint8_t *)name_2) == 0){
		return FAIL;
	}
	if(file_open((uint8_t *)name_3) == 0){
		return FAIL;
	}

	/* Test file_read */
	if(file_read(fd, &buf, nbytes, (uint8_t *)name_1) == -1){
		return FAIL;
	}
	if(file_read(fd, &buf, nbytes, (uint8_t *)name_2) == 0){
		return FAIL;
	}

	/* Test file_write */
	if(file_write(fd, &buf, nbytes) != -1){
		return FAIL;
	}

	/* Test file_close */
	if(file_close(fd) != 0){
		return FAIL;
	}
	printf("\n");
	/* Passed tests */
  return PASS;
}

/* dir_syscalls_test() 
*		Description: tests the directory system calls in the following order:
					 - dir_open
					 - dir_read
					 - dir_write
					 - dir_close
*		Author: Austin
*		Input: None
*		Output: PASS for success, FAIL for failure
*		Side effects: Clears terminal, prints to terminal
*/
int dir_syscalls_test() {
	clear();

	TEST_HEADER;
	

	uint8_t i;
	char* name_1;
	char* name_2;
	char* name_3;
	uint8_t buf[BUF_SIZE];
	uint8_t fd;
	int32_t nbytes;
	nbytes = BUF_SIZE;

	name_1 = "cat";
	name_2 = "notafile";
	name_3 = ".";
	fd = 0;

	/* Test directory_open */
	if(directory_open((uint8_t *)name_1) == 0){
		return FAIL;
	}
	if(directory_open((uint8_t *)name_2) == 0){
		return FAIL;
	}
	if(directory_open((uint8_t *)name_3) == -1){
		return FAIL;
	}

	/* Test directory_read */
	if(directory_read(fd, &buf, nbytes) == 0){
		return FAIL;
	}
	printf("%s\n", buf);
	memset(&buf, '\0', FILE_NAME_SIZE);

	while(directory_read(fd, &buf, FILE_NAME_SIZE) != 0){
		printf("%s\n", buf);
		memset(&buf, '\0', FILE_NAME_SIZE);
	}

	i = 0;
	for(i = 0; i < 20; i++){
		if(directory_read(fd, &buf, nbytes) != 0){
			return FAIL;
		}
	}

	/* Test directory_write */
	if(directory_write(fd, &buf, nbytes) != -1){
		return FAIL;
	}

	/* Test directory_close */
	if(directory_close(fd) != 0){
		return FAIL;
	}
	printf("\n");
	/* Passed tests */
  return PASS;
}

/* test_read_dentry_by_name()
*		Description: tests the read_dentry_by_name function with the following tests:
								 - Regular file
								 - File doesn't exist
								 - File has large name
								 - File name points to NULL
*		Author: Hershel and Austin
*		Input: None
*		Output: PASS for success, FAIL for failure
*		Side effects: Clears terminal, prints dentry info
*/
int test_read_dentry_by_name() {
	clear();	
	TEST_HEADER;


 	dentry_t test;
 	char* name;
 	uint8_t buf[BUF_SIZE];

 /* Regular file test */
  name = "cat";
  if(read_dentry_by_name((uint8_t *)name, &(test)) == 0) {
    printf("File name is: %s\n", test.file_name);
    printf("File type is: %d\n", test.file_type);
		printf("Inode number is: %d\n", test.inode_number);
  }
  else {
	printf("Could not find existing file \n");
	return FAIL;
  }

  /* File doesn't exist */
  name = "UIUC";
  if(read_dentry_by_name((uint8_t *)name, &(test)) == 0) {
    printf("File name is: %s\n", test.file_name);
    printf("File type is: %d\n", test.file_type);
		printf("Inode number is: %d\n", test.inode_number);
		printf("Found non-existent file \n");
	return FAIL;
  }
  else {
	printf(" Non-existent file ignored \n");
  }

  /* File has large name */
  name = "verylargetextwithverylongname.tx";
  if(read_dentry_by_name((uint8_t *)name, &(test)) == 0) {
		strncpy((int8_t *)buf, (int8_t *)&(test.file_name), FILE_NAME_SIZE);
    printf("File name is: %s\n", buf);
		memset(&buf, '\0', FILE_NAME_SIZE);
    printf("File type is: %d\n", test.file_type);
		printf("Inode number is: %d\n", test.inode_number);
  }
  else {
		printf("Couldn't match full name \n");
		return FAIL;
  }

  /* File name is NULL */
  name = NULL;
  if(read_dentry_by_name((uint8_t *)name, &(test)) == 0) {
    printf("File name is: %s\n", test.file_name);
    printf("File type is: %d\n", test.file_type);
		printf("Inode number is: %d\n", test.inode_number);
		printf("NULL file name found \n");
		return FAIL;
  }
  else {
		printf("NULL file name ignored \n");
  }
	printf("\n");
	/* Passed tests */
  return PASS;
}

/* test_read_dentry_by_index()
*		Description: tests the read_dentry_by_index function with the following tests:
								 - Active file indices
								 - Index above active indices
								 - Index below 0
*		Author: Hershel and Austin
*		Input: None
*		Output: PASS for success, FAIL for failure
*		Side effects: Clears terminal, prints dentry info
*/
int test_read_dentry_by_index() {
	//clear();
	TEST_HEADER;
  uint32_t index = 0;
  uint32_t retval = 0;
  dentry_t test;
	uint8_t buf[BUF_SIZE];

	/* Test all active indices */
  for(index = 0; index < number_of_files; index++) {
    retval = read_dentry_by_index(index, &test);
    if(retval == -1) {
      return FAIL;
  	}
  	else {
			strncpy((int8_t *)buf, (int8_t *)&(test.file_name), FILE_NAME_SIZE);
			printf("File name is: %s\n", buf);
			memset(&buf, '\0', FILE_NAME_SIZE);
			printf("File type is: %d\n", test.file_type);
			printf("Inode number is: %d\n", test.inode_number);
    }
	}
	
	/* Test index above number of files */
	retval = read_dentry_by_index(number_of_files + 1, &test);
	if(retval == -1) {
		printf("Index > number_of_files -> No file \n");
	}
	else {
		printf("Index > number_of_files is not a file! \n");
		printf("File name is: %s\n", test.file_name);
		return FAIL;
	}

	/* Test index below first file */
	retval = read_dentry_by_index(-1, &test);
	if(retval == -1) {
		printf("Index < 0 -> No file \n");
	}
	else {
		printf("Index < 0 is not a file! \n");
		printf("File name is: %s\n", test.file_name);
		return FAIL;
	}
	printf("\n");
	/* Passed tests */
  return PASS;
}

/* test_read_data()
*		Description: tests the read_data function with the following tests (by selection):
								 - File with large name
								 - Text file
								 - Object file
*		Author: Hershel and Austin
*		Input: None
*		Output: PASS for success, FAIL for failure
*		Side effects: Clears terminal, prints file data
*/
int test_read_data() {
	clear();
	TEST_HEADER;
	int i = 0;
	int retval;
	uint8_t buf[BUF_SIZE];
	dentry_t file_dentry;

	/* Test to fetch data using file name (comment out unused file names) */
	//char* file = "verylargetextwithverylongname.tx";
	char* file = "frame0.txt";
	//char* file = "pingpong";

	retval = read_dentry_by_name((uint8_t*)file, &(file_dentry));

	inode_t* this_inode;
	this_inode = (inode_t*)((uint8_t*)filesystem.inode_start + (file_dentry.inode_number * BLOCK_SIZE));
	this_inode->length = *((uint32_t*)this_inode);

	uint32_t length = this_inode->length;

	read_data(file_dentry.inode_number, 0, (uint8_t *)&buf, length);

	for(i = 0; i < length; i++) {
		terminal_write(0,buf+i,1);
	}
	printf("\n");
	/* Passed test */
	return PASS;
}

/* test_read_dir()
*		Description: tests the directory_read function with the following tests:
								 - Regular files and directory dentry
								 - Exits upon reaching end of directory
*		Author: Hershel and Austin
*		Input: None
*		Output: PASS for success, FAIL for failure
*		Side effects: Clears terminal, prints successive dentries
*/
int test_read_dir() {
	uint8_t buf[BUF_SIZE];
	//clear();

	/*Test directory_read() repeatedly until end of directory is reached */
	while(directory_read(0, &buf, FILE_NAME_SIZE) != 0) {
		printf("%s\n", buf);
		memset(&buf, '\0', FILE_NAME_SIZE);
	}

	/* Passed tests */
	return PASS;

}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());

		// launch your tests here
	//TEST_OUTPUT("paging_test", paging_test());
	TEST_OUTPUT("terminal_driver_test", terminal_driver_test());
	TEST_OUTPUT("RTC TEST", rtc_test());
	//TEST_OUTPUT("dentry_by_name_test", test_read_dentry_by_name());
	//TEST_OUTPUT("dentry_by_index_test", test_read_dentry_by_index());
	//TEST_OUTPUT("read_data_test", test_read_data());
	//TEST_OUTPUT("dir_read_test", test_read_dir());
	//TEST_OUTPUT("file_syscalls_test", file_syscalls_test());
	//TEST_OUTPUT("dir_syscalls_test", dir_syscalls_test());
	//TEST_OUTPUT("div_zero_test", div_zero_test());
	return;
}
