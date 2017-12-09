//include defines etc.
#include "x86_desc.h"
#include "idt_init.h"
#include "lib.h"
#include "types.h"
#include "i8259.h"
#include "keyboard.h"
#include "rtc.h"
#include "exception_link.H"
#include "sys_call_link.H"
#include "pit.h"

int i; // loop variable
/*
*	idt_init
*		Author: Sam
*		Description:  This function is called in boot.S to initaialize and fill the idt
*									Each entry is first just filled (only important thing is that .present = 0)
*						  		IDT entries for ecxceptions, sys calls, and i/o handlers are filled
*		Inputs: None
*		Outputs: none
* 	Returns: nothing
*		Side effects: The idt is initialized and filled as appropriate.
*/
void idt_init() {
	/*Initialize all IDT entries to default unused*/
	for(i = 0; i < IDT_SIZE; i++){
		idt[i].seg_selector = KERNEL_CS;
		//set reserved bits
		idt[i].reserved0 = 0;
		idt[i].reserved1 = 0;
		idt[i].reserved2 = 0;
		idt[i].reserved3 = 0;
		idt[i].reserved4 = 0;
		//set size bit
		idt[i].size = 0;
		//set dpl bits
		idt[i].dpl = 0;
		//set present flag
		idt[i].present = 0;
	}

	/*Set exception IDT entries (using trap settings)*/
	for(i = 0; i < NUM_EXCEPTIONS; i ++){
		//set seg_selector bits
		idt[i].seg_selector = KERNEL_CS;

		/* set as trap get descriptor */
		//set reserved bits
		idt[i].reserved0 = 0;
		idt[i].reserved1 = 1;
		idt[i].reserved2 = 1;
		idt[i].reserved3 = 1;
		idt[i].reserved4 = 0;
		//set size bit
		idt[i].size = 1;

		//set dpl bits
		idt[i].dpl = 0;
		idt[i].present = 1;
	}

	/*
		 Fill in exception handlers in locations defined by intel
		 and add 15 to be an assertion error
	*/
		SET_IDT_ENTRY(idt[0], &divide_by_zero_asm);
		SET_IDT_ENTRY(idt[1], &debug_asm);
		SET_IDT_ENTRY(idt[2], &nmi_interrupt_asm);
		SET_IDT_ENTRY(idt[3], &breakpoint_asm);
		SET_IDT_ENTRY(idt[4], &overflow_asm);
		SET_IDT_ENTRY(idt[5], &bound_range_exceeded_asm);
		SET_IDT_ENTRY(idt[6], &invalid_opcode_asm);
		SET_IDT_ENTRY(idt[7], &device_not_available_asm);
		SET_IDT_ENTRY(idt[8], &double_fault_asm);
		SET_IDT_ENTRY(idt[9], &coprocessor_segment_overrun_asm);
		SET_IDT_ENTRY(idt[10], &invalid_tss_asm);
		SET_IDT_ENTRY(idt[11], &segment_not_present_asm);
		SET_IDT_ENTRY(idt[12], &stack_segment_asm);
		SET_IDT_ENTRY(idt[13], &general_protection_asm);
		SET_IDT_ENTRY(idt[14], &page_fault_asm);
		SET_IDT_ENTRY(idt[15], &assertion_fail_asm);
		SET_IDT_ENTRY(idt[16], &fpu_floating_point_exception_asm);
		SET_IDT_ENTRY(idt[17], &alignment_check_asm);
		SET_IDT_ENTRY(idt[18], &machine_check_asm);
		SET_IDT_ENTRY(idt[19], &simd_floating_point_exception_asm);



		/*Initialize system call IDT entries*/
		idt[SYSTEM_CALL_IDT_ENTRY].seg_selector = KERNEL_CS;
		//set reserved bits
		idt[SYSTEM_CALL_IDT_ENTRY].reserved0 = 0;
		idt[SYSTEM_CALL_IDT_ENTRY].reserved1 = 1;
		idt[SYSTEM_CALL_IDT_ENTRY].reserved2 = 1;
		idt[SYSTEM_CALL_IDT_ENTRY].reserved3 = 0;
		idt[SYSTEM_CALL_IDT_ENTRY].reserved4 = 0;
		//set size bit
		idt[SYSTEM_CALL_IDT_ENTRY].size = 1;
		//set dpl bits
		idt[SYSTEM_CALL_IDT_ENTRY].dpl = USER_PROTECTION;
		//Ignore initialization for 15
		idt[SYSTEM_CALL_IDT_ENTRY].present = 1;
		SET_IDT_ENTRY(idt[SYSTEM_CALL_IDT_ENTRY], sys_call_asm);


		/* Initialize PIT interrupt IDT entry */
		idt[PIT_IDT_ENTRY].seg_selector = KERNEL_CS;
		idt[PIT_IDT_ENTRY].reserved0 = 0;
		idt[PIT_IDT_ENTRY].reserved1 = 1;
		idt[PIT_IDT_ENTRY].reserved2 = 1;
		idt[PIT_IDT_ENTRY].reserved3 = 0;
		idt[PIT_IDT_ENTRY].reserved4 = 0;
		idt[PIT_IDT_ENTRY].size = 1;
		idt[PIT_IDT_ENTRY].dpl = 0;
		idt[PIT_IDT_ENTRY].present = 1;
		SET_IDT_ENTRY(idt[PIT_IDT_ENTRY], &pit_hlp);
		/*Initialize keyboard interrupt IDT entry*/
		idt[KEYBOARD_IDT_ENTRY].seg_selector = KERNEL_CS;
		//set reserved bits
		idt[KEYBOARD_IDT_ENTRY].reserved0 = 0;
		idt[KEYBOARD_IDT_ENTRY].reserved1 = 1;
		idt[KEYBOARD_IDT_ENTRY].reserved2 = 1;
		idt[KEYBOARD_IDT_ENTRY].reserved3 = 0;
		idt[KEYBOARD_IDT_ENTRY].reserved4 = 0;
		//set size bit
		idt[KEYBOARD_IDT_ENTRY].size = 1;
		//set dpl bits

		idt[KEYBOARD_IDT_ENTRY].dpl = 0;

		//WHYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY??
		idt[0x21].dpl = 3;

		//Ignore initialization for 15
		idt[KEYBOARD_IDT_ENTRY].present = 1;
		SET_IDT_ENTRY(idt[KEYBOARD_IDT_ENTRY], &get_char);

		/*Initialize RTC interrupt IDT entry*/
		idt[RTC_IDT_ENTRY].seg_selector = KERNEL_CS;
		//set reserved bits
		idt[RTC_IDT_ENTRY].reserved0 = 0;
		idt[RTC_IDT_ENTRY].reserved1 = 1;
		idt[RTC_IDT_ENTRY].reserved2 = 1;
		idt[RTC_IDT_ENTRY].reserved3 = 0;
		idt[RTC_IDT_ENTRY].reserved4 = 0;
		//set size bit
		idt[RTC_IDT_ENTRY].size = 1;
		//set dpl bits
		idt[RTC_IDT_ENTRY].dpl = 0;
		//Ignore initialization for 15
		idt[RTC_IDT_ENTRY].present = 1;
		SET_IDT_ENTRY(idt[RTC_IDT_ENTRY], &rtc_int);

	lidt (idt_desc_ptr);

	return;
}

/* assembly linkage */
void sys_call(){
	__asm__("pusha\n\t"
			"call sys_call_hlp\n\t"
			"popa\n\t"
			"leave\n\t"
			"IRET\n\t");
}

void pit_hlp(){
	__asm__("pusha\n\t"
		"call pit_handler\n\t"
		"popa\n\t"
		"leave\n\t"
		"IRET\n\t");
}

void get_char(){
	__asm__("pusha\n\t"
			"cli\n\t"
			"call keyboard_handler\n\t"
			"sti\n\t"
			"popa\n\t"
			"leave\n\t"
			"IRET\n\t");
}

void rtc_int(){
	__asm__("pusha\n\t"
			"cli\n\t"
			"call rtc_handler\n\t"
			"sti\n\t"
			"popa\n\t"
			"leave\n\t"
			"IRET\n\t");
}

void sys_call_hlp(){
	printf("This is a system call\n");
	while(1);
}

