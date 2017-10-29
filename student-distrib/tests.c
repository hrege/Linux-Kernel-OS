#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt_init.h"
#include "rtc.h"
 
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

//TEST #3 - RTC TEST
int rtc_test(){
	printf("\nThis test will print the character '1' when an RTC interrupt occurs \nas determined using rtc_read\nStarting with rtc_open which will set it to 2Hz\n");
	rtc_open(NULL);
	rtc_helper(10);
	int i; //to hold return vals
	printf("Calling RTC write with rate of 64Hz\n");
	i = rtc_write(0, NULL, 64);
	if(i==-1){
		return FAIL;
	}
	rtc_helper(120);
	printf("Calling RTC write with rate of 420Hz\n");
	i = rtc_write(0, NULL, 420);
	if(i==0){
		return FAIL;
	}
	rtc_helper(120);
	printf("Calling RTC write with rate of 1024Hz\n");
	i = rtc_write(0, NULL, 1024);
	if(i==-1){
		return FAIL;
	}
	rtc_helper(2000);
		printf("Calling RTC write with rate of 2Hz\n");
	i = rtc_write(0, NULL, 2);
	if(i==-1){
		return FAIL;
	}
	rtc_helper(8);
	return PASS;

}
void rtc_helper(int i){
	while(i>0){
		rtc_read(0, NULL, 0);
		printf("1");
		i--;
	}
	printf("\n");
	return;
}
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("paging_test", paging_test());
	//TEST_OUTPUT("div_zero_test", div_zero_test());
	TEST_OUTPUT("RTC TEST", rtc_test());

	// launch your tests here
	return;
}
