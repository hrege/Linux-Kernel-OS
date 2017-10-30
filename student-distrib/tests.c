#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt_init.h"
#include "keyboard.h"
#include "rtc.h"

 
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
	set_screen_x(0);	//reset to top left
	set_screen_y(0);
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
		set_screen_x(0);	//reset to top left
		set_screen_y(0);
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

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("paging_test", paging_test());
	TEST_OUTPUT("terminal_driver_test", terminal_driver_test());

	//TEST_OUTPUT("div_zero_test", div_zero_test());
	TEST_OUTPUT("RTC TEST", rtc_test());

	// launch your tests here
	return;
}
