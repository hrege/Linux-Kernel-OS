#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

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

/* Doesn't actually test anything yet
int keyboard_test(){
	TEST_HEADER;
	printf("PLZ WORK MR. KEYBOARD");
	while(1);
	return 0;
}
*/

/* div_zero_test
*		Description: tezts the divide by zero exception handler being called and such
*		Author: Jonathan
*		Input: None
*		Output: None
*		Returns: always FAIL because you shouldn't reach return
*		Side effects: tries to do '5 / 0' which should result in reaching the divide by zero excpetion handler
*/
int div_zero_test(){
	TEST_HEADER;
	int i;
	i=5/0;
	return FAIL;
}

int paging_test(){
	TEST_HEADER;
	
	return PASS;
}

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("div_zero_test", div_zero_test());
	//TEST_OUTPUT("keyboard_test", keyboard_test());
	// launch your tests here
	return;
}
