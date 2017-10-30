/*
*	C Exception Handlers. 
*	These handlers are linked to from the idt through assembly linkage in exception_link.S
*	Handlers originally created by Sam
*	Reorganized and put in separate file by Jonathan
*	If/when handlers are more filled out check funtion interfaces for authors
*	At this point functions only print the name of the exception so one template interface is
*	provided at the top for divide by zero exception
*
*			VictoriOS Secret
*
*/

#include "lib.h"
#include "types.h"
#include "exceptions.h"

/*
*	divide_by_zero
*		Description: C Handler for the divide by zero exception
*						-Currently doesn't do much
*		Input: none
*		Output: none
*		Return: Doesn't
*		Side-effects: Prints the name of the exception reached
*
*/

void divide_by_zero() {
	printf("divide_by_zero\n");
	while(1);
}

//vector # 1 reserved for Intel use
void debug(){
	printf("debug exception\n");
	while(1);
}

void nmi_interrupt(){
	printf("nmi_interrupt\n");
	while(1);
}

void breakpoint(){
	printf("breakpoint\n");
	while(1);
}

void overflow(){
	printf("divide_by_zero\n");
	while(1);
}

void bound_range_exceeded(){
	printf("bound_range_exceeded\n");
	while(1);
}

void invalid_opcode(){
	printf("invalid_opcode\n");
	while(1);
}


void device_not_available(){
	printf("device_not_available\n");
	while(1);
}

void double_fault(){
	printf("this is a double_fault\n");
	while(1);
}

void coprocessor_segment_overrun(){
	printf("coprocessor_segment_overrun\n");
	while(1);
}

void invalid_tss(){
	printf("invalid_tss\n");
	while(1);
}

void segment_not_present(){
	printf("segment_not_present\n");
	while(1);
}

void stack_segment(){
	printf("stack_segment_fault\n");
	while(1);
}

void general_protection(){
	printf("general_protection\n");
	while(1);

}

void page_fault(){
	printf("page_fault\n");
	while(1);
}

//vector 15 reserved for Intel use
void assertion_fail(){
	printf("Assertion failure\n");
	while(1);
}

void fpu_floating_point_exception(){
	printf("floating_point_exception\n");
	while(1);
}

void alignment_check(){
	printf("alignment_check\n");
	while(1);
}

void machine_check(){
	printf("machine_check\n");
	while(1);
}

void simd_floating_point_exception(){
	printf("divide_by_zero\n");
	while(1);
}
