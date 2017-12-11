/*
*	C Exception Handlers. 
*		"Author": Jonthan
*	These handlers are linked to from the idt through assembly linkage in exception_link.S
*
*/


#include "lib.h"
#include "types.h"
#include "exceptions.h"
#include "sys_call.h"

/*
*	divide_by_zero
*		Description: C Handler for the divide by zero exception
*						-Will print exception type and halt process
*		Input: none
*		Output: none
*		Return: Doesn't
*		Side-effects: Prints the name of the exception reached. process halted
*
*/

void divide_by_zero() {
	printf("divide_by_zero\n");
	sys_halt((uint8_t)HALT_STATUS);
}

//vector # 1 reserved for Intel use
void debug(){
	printf("debug exception\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void nmi_interrupt(){
	printf("nmi_interrupt\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void breakpoint(){
	printf("breakpoint\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void overflow(){
	printf("divide_by_zero\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void bound_range_exceeded(){
	printf("bound_range_exceeded\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void invalid_opcode(){
	printf("invalid_opcode\n");
	sys_halt((uint8_t)HALT_STATUS);
}


void device_not_available(){
	printf("device_not_available\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void double_fault(){
	printf("this is a double_fault\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void coprocessor_segment_overrun(){
	printf("coprocessor_segment_overrun\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void invalid_tss(){
	printf("invalid_tss\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void segment_not_present(){
	printf("segment_not_present\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void stack_segment(){
	printf("stack_segment_fault\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void general_protection(){
	printf("general_protection\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void page_fault(){
	printf("page_fault\n");
	sys_halt((uint8_t)HALT_STATUS);
}

//vector 15 reserved for Intel use
void assertion_fail(){
	printf("Assertion failure\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void fpu_floating_point_exception(){
	printf("floating_point_exception\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void alignment_check(){
	printf("alignment_check\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void machine_check(){
	printf("machine_check\n");
	sys_halt((uint8_t)HALT_STATUS);
}

void simd_floating_point_exception(){
	printf("divide_by_zero\n");
	sys_halt((uint8_t)HALT_STATUS);
}
