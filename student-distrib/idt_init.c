//include defines etc.
#include "x86_desc.h"

int i, addr; // loop variable
char exceptions[20];


void idt_init() {
	for(i = 0; i < 20; i ++){

		if(i != 15){
		//set seg_selector bits
		idt[i].seg_selector = gdt_ptr; //gedt_desc_ptr
		//set reserved bits
		idt[i].reserved0 = 0;
		idt[i].reserved1 = 1;
		idt[i].reserved2 = 1;
		idt[i].reserved3 = 0;
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
		SET_IDT_ENTRY(idt[12], &stack_segment_fault);
		SET_IDT_ENTRY(idt[13], &general_protection);
		SET_IDT_ENTRY(idt[14], &page_fault);
		SET_IDT_ENTRY(idt[16], &fpu_floating_point_exception);
		SET_IDT_ENTRY(idt[17], &alignment_check);
		SET_IDT_ENTRY(idt[18], &machine_check);
		SET_IDT_ENTRY(idt[19], &simd_floating_point_exception);


	lidt (idt_desc_ptr);

	return;

}
