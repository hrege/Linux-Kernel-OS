#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt_init.h"
#include "filesystem.h"

#define PASS 1
#define FAIL 0

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
int test_read_dentry_by_name() {
  TEST_HEADER;

 dentry_t test;
 char* name;

 /* Regular file test */
  name = "cat";
  if(read_dentry_by_name((uint8_t *)name, &(test)) == 0) {
    printf("%s\n", test.file_name);
    printf("%d\n", test.file_type);
	printf("%d\n", test.inode_number);
  }
  else {
	printf("Could not find existing file \n");
	return FAIL;
  }

  /* File doesn't exist */
  name = "UIUC";
  if(read_dentry_by_name((uint8_t *)name, &(test)) == 0) {
    printf("%s\n", test.file_name);
    printf("%d\n", test.file_type);
	printf("%d\n", test.inode_number);
	printf("Found non-existent file \n");
	return FAIL;
  }
  else {
	printf(" Non-existent file ignored \n");
  }

  /* File has large name */
  name = "verylargetextwithverylongname.tx";
  if(read_dentry_by_name((uint8_t *)name, &(test)) == 0) {
    printf("%s\n", test.file_name);
    printf("%d\n", test.file_type);
	printf("%d\n", test.inode_number);
  }
  else {
	printf("Couldn't match full name \n");
	return FAIL;
  }

  /* File name is NULL */
  name = NULL;
  if(read_dentry_by_name((uint8_t *)name, &(test)) == 0) {
    printf("%s\n", test.file_name);
    printf("%d\n", test.file_type);
	printf("%d\n", test.inode_number);
	printf("NULL file name found \n");
	return FAIL;
  }
  else {
	printf("NULL file name ignored \n");
  }

  return PASS;
}

int test_read_dentry_by_index() {
	clear();
	TEST_HEADER;
  uint32_t index = 0;
  uint32_t retval = 0;
  dentry_t test;

  for(index = 0; index < number_of_files; index++) {
    retval = read_dentry_by_index(index, &test);
    if(retval == -1) {
      return FAIL;
  	}
  	else {
      printf("%s\n", test.file_name);
      //printf("%d\n", test.file_type);
      //printf("%d\n", test.inode_number);
    }
  }
	retval = read_dentry_by_index(number_of_files + 1, &test);
	if(retval == -1) {
		printf("Greater than number of files\n");
	}
	else {
		printf("Index > number_of_files is not a file!\n");
		printf("%s\n", test.file_name);
		return FAIL;
	}

	retval = read_dentry_by_index(-1, &test);
	if(retval == -1) {
		printf("Found negative index!\n");
	}
	else {
		printf("Incorrectly found negative index\n");
		return FAIL;
	}

  return PASS;
}

int test_read_data() {
	int i = 0;
	uint8_t buf[10000];
	uint32_t length = 10000;
	int32_t fd = 0;
	char* file = "verylargetextwithverylongname.tx";
	if(file_read(fd, buf, length, (uint8_t*)file) == length) {
		for(i = 0; i < length; i++) {
			printf("%c\n", buf[i]);
		}
		return PASS;
	}
	else {
		return FAIL;
	}
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("paging_test", paging_test());
  //TEST_OUTPUT("dentry_by_name_test", test_read_dentry_by_name());
  //TEST_OUTPUT("dentry_by_index_test", test_read_dentry_by_index());
	TEST_OUTPUT("read_data_test", test_read_data());
  //TEST_OUTPUT("div_zero_test", div_zero_test());

	// launch your tests here
	return;
}
