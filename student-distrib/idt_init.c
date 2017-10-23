//include defines etc.
#include "x86_desc.h"
#include "idt_init.h"
#include "lib.h"
#include "types.h"

int i, addr; // loop variable
char exceptions[20];


void idt_init() {
	/*Initialize all IDT entries to default unused*/
	for(i = 0; i < 256; i++){
		idt[i].seg_selector = 0; //gedt_desc_ptr
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
		//Ignore initialization for 15
			idt[i].present = 0;


	}
	/*Set exception IDT entries*/
	for(i = 0; i < 20; i ++){

		if(i != 15){
		//set seg_selector bits
		idt[i].seg_selector = KERNEL_CS; 
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
		//Ignore initialization for 15
			idt[i].present = 1;
		}
	}


		SET_IDT_ENTRY(idt[0], &divide_by_zero);
		SET_IDT_ENTRY(idt[1], &debug);
		SET_IDT_ENTRY(idt[2], &nmi_interrupt);
		SET_IDT_ENTRY(idt[3], &breakpoint);
		SET_IDT_ENTRY(idt[4], &overflow);
		SET_IDT_ENTRY(idt[5], &bound_range_exceeded);
		SET_IDT_ENTRY(idt[6], &invalid_opcode);
		SET_IDT_ENTRY(idt[7], &device_not_available);
		SET_IDT_ENTRY(idt[8], &double_fault);
		SET_IDT_ENTRY(idt[9], &coprocessor_segment_overrun);
		SET_IDT_ENTRY(idt[10], &invalid_tss);
		SET_IDT_ENTRY(idt[11], &segment_not_present);
		SET_IDT_ENTRY(idt[12], &stack_segment);
		SET_IDT_ENTRY(idt[13], &general_protection);
		SET_IDT_ENTRY(idt[14], &page_fault);
		SET_IDT_ENTRY(idt[16], &fpu_floating_point_exception);
		SET_IDT_ENTRY(idt[17], &alignment_check);
		SET_IDT_ENTRY(idt[18], &machine_check);
		SET_IDT_ENTRY(idt[19], &simd_floating_point_exception);

		/*Initialize interrupt IDT entries*/

		/*Initialize system call IDT entries*/
		idt[0x80].seg_selector = KERNEL_CS; 
		//set reserved bits
		idt[0x80].reserved0 = 0;
		idt[0x80].reserved1 = 1;
		idt[0x80].reserved2 = 1;
		idt[0x80].reserved3 = 1;
		idt[0x80].reserved4 = 0;
		//set size bit
		idt[0x80].size = 1;
		//set dpl bits
		idt[0x80].dpl = 3;
		//Ignore initialization for 15
		idt[0x80].present = 1;
		SET_IDT_ENTRY(idt[0x80], &sys_call);

		/*Initialize keyboard interrupt IDT entry*/
		idt[0x21].seg_selector = KERNEL_CS; 
		//set reserved bits
		idt[0x21].reserved0 = 0;
		idt[0x21].reserved1 = 1;
		idt[0x21].reserved2 = 1;
		idt[0x21].reserved3 = 0;
		idt[0x21].reserved4 = 0;
		//set size bit
		idt[0x21].size = 1;
		//set dpl bits
		idt[0x21].dpl = 0;
		//Ignore initialization for 15
		idt[0x21].present = 1;
		SET_IDT_ENTRY(idt[0x21], &get_char);


		/*Initialize RTC interrupt IDT entry*/
		idt[0x28].seg_selector = KERNEL_CS; 
		//set reserved bits
		idt[0x28].reserved0 = 0;
		idt[0x28].reserved1 = 1;
		idt[0x28].reserved2 = 1;
		idt[0x28].reserved3 = 0;
		idt[0x28].reserved4 = 0;
		//set size bit
		idt[0x28].size = 1;
		//set dpl bits
		idt[0x28].dpl = 0;
		//Ignore initialization for 15
		idt[0x28].present = 1;
		SET_IDT_ENTRY(idt[0x28], &rtc_int);


	lidt (idt_desc_ptr);

	return;

}



/*Assembly linkage for C handlers*/
void divide_by_zero(){
	__asm__("pusha\n\t"
			"call divide_by_zero_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}

//vector # 1 reserved for Intel use
void debug(){
	__asm__("pusha\n\t"
			"call debug_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}

void nmi_interrupt(){
	__asm__("pusha\n\t"
			"call nmi_interrupt_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}

void breakpoint(){
	__asm__("pusha\n\t"
			"call breakpoint_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}

void overflow(){
	__asm__("pusha\n\t"
			"call overflow_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}


void bound_range_exceeded(){
	__asm__("pusha\n\t"
			"call bound_range_exceeded_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}


void invalid_opcode(){
	__asm__("pusha\n\t"
			"call invalid_opcode_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}


void device_not_available(){
	__asm__("pusha\n\t"
			"call device_not_available_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}

void double_fault(){
	__asm__("pusha\n\t"
			"call double_fault_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}


void coprocessor_segment_overrun(){
	__asm__("pusha\n\t"
			"call coprocessor_segment_overrun_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}


void invalid_tss(){
	__asm__("pusha\n\t"
			"call invalid_tss_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}

void segment_not_present(){
	__asm__("pusha\n\t"
			"call segment_not_present_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}

void stack_segment(){
	__asm__("pusha\n\t"
			"call stack_segment_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}

void general_protection(){
	__asm__("pusha\n\t"
			"call general_protection_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}

void page_fault(){
	__asm__("pusha\n\t"
			"call page_fault_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}

//vector 15 reserved for Intel use

void fpu_floating_point_exception(){
	__asm__("pusha\n\t"
			"call fpu_floating_point_exception_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}


void alignment_check(){
	__asm__("pusha\n\t"
			"call alignment_check_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}

void machine_check(){
	__asm__("pusha\n\t"
			"call machine_check_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}

void simd_floating_point_exception(){
	__asm__("pusha\n\t"
			"call simd_floating_point_exception_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}

void sys_call(){
	__asm__("pusha\n\t"
			"call sys_call_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}

void get_char(){
	__asm__("pusha\n\t"
			"call get_char_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");


}

void rtc_int(){
	__asm__("pusha\n\t"
			"call rtc_int_hlp\n\t"
			"popa\n\t"
			"IRET\n\t");

}


/*C handlers*/
/*
*
*
*
*
*
*
*
*
*
*
*
*
*
*
*/

void divide_by_zero_hlp(){
	printf("divide_by_zero\n");



}

//vector # 1 reserved for Intel use
void debug_hlp(){
	printf("debug exception\n");
	while(1);

}

void nmi_interrupt_hlp(){
	printf("nmi_interrupt\n");
	while(1);

}

void breakpoint_hlp(){
	printf("breakpoint\n");
	while(1);

}

void overflow_hlp(){
	printf("divide_by_zero\n");
	while(1);

}


void bound_range_exceeded_hlp(){
	printf("bound_range_exceeded\n");
	while(1);

}


void invalid_opcode_hlp(){
	printf("invalid_opcode\n");
	while(1);

}


void device_not_available_hlp(){
	printf("device_not_available\n");
	while(1);

}

void double_fault_hlp(){
	printf("this is a double_fault\n");
	while(1);

}


void coprocessor_segment_overrun_hlp(){
	printf("coprocessor_segment_overrun\n");
	while(1);

}


void invalid_tss_hlp(){
	printf("invalid_tss\n");
	while(1);

}

void segment_not_present_hlp(){
	printf("segment_not_present\n");
	while(1);

}

void stack_segment_hlp(){
	printf("stack_segment_fault\n");
	while(1);

}

void general_protection_hlp(){
	printf("general_protection\n");
	//while(1);

}

void page_fault_hlp(){
	printf("page_fault\n");
	while(1);

}

//vector 15 reserved for Intel use

void fpu_floating_point_exception_hlp(){
	printf("floating_point_exception\n");
	while(1);

}


void alignment_check_hlp(){
	printf("alignment_check\n");
	while(1);

}

void machine_check_hlp(){
	printf("machine_check\n");
	while(1);

}

void simd_floating_point_exception_hlp(){
	printf("divide_by_zero\n");
	while(1);

}

void sys_call_hlp(){
	printf("This is a system call\n");

}

void get_char_hlp(){
	char keyboard_input;
	keyboard_input = inb(0x60);
	printf(keyboard_input);
	printf("\n");

	send_eoi(0x21);

	
}

void rtc_int_hlp(){
	printf("This is an RTC interrupt\n");
	send_eoi(0x28);

}






