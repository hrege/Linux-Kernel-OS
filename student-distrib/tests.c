#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt_init.h"
 
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





/* div_zero_test
*		Description: tezts the divide by zero exception handler being called and such
*		Author: Jonathan
*		Input: None
*		Output: None
*		Returns: always FAIL because you shouldn't reach return
*		Side effects: tries to do '5 / 0' which should result in reaching the divide by zero excpetion handler
*/


// 	TEST_HEADER;
// 	int i;
// 	i=5/0;
// 	return FAIL;
// }

int paging_test(){
	TEST_HEADER;
	int* kernel_cs_test = 0x0400019B;
	int* vid_mem_test = 0xB811B;
	int* null_test = NULL;

	printf("%d\n", *(kernel_cs_test));
	printf("%d\n", *(vid_mem_test));	
	printf("%d\n", *(null_test));
 	return PASS;
 }

void keyboard_test(){
	TEST_HEADER;
	// array        [i   ,   f,    ,   y,   o,   u,   r,   e,    ,   r,   e,   a,   d,   i,   n,   g,    ,   t,   h,   i,   s,    ,   i,   t,   s,    ,   a,    ,   p,   a,   s,   s]
	char string[] = [0x17,0x21,0x39,0x15,0x18,0x16,0x13,0x12,0x39,0x13,0x12,0x1E,0x20,0x17,0x31,0x22,0x39,0x14,0x23,0x17,0x1F,0x39,0x17,0x14,0x1F,0x39,0x1E,0x39,0x19,0x1E,0x1F,0x1F]
	int i; //counter
	for(i = 0; i < 35; i++){
		outb(string[i]; 0x60);
		asm volatile("int $21")
	}
	return;
	}

}
/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	TEST_OUTPUT("paging_test", paging_test());
	keyboard_test();
	//TEST_OUTPUT("div_zero_test", div_zero_test());
	//TEST_OUTPUT("keyboard_test", keyboard_test());
	// launch your tests here
	return;
}
