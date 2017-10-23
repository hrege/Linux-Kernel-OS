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
		idt[0x80].reserved3 = 0;
		idt[0x80].reserved4 = 0;
		//set size bit
		idt[0x80].size = 1;
		//set dpl bits
		idt[0x80].dpl = 3;
		//Ignore initialization for 15
		idt[0x80].present = 1;
		SET_IDT_ENTRY(idt[0x80], &sys_call);

		/*Initialize interrupt IDT entries*/
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




void divide_by_zero(){
	printf("divide_by_zero");
	while(1);

}

//vector # 1 reserved for Intel use
void debug(){
	printf("debug exception");
	while(1);

}

void nmi_interrupt(){
	printf("nmi_interrupt");
	while(1);

}

void breakpoint(){
	printf("breakpoint");
	while(1);

}

void overflow(){
	printf("divide_by_zero");
	while(1);

}


void bound_range_exceeded(){
	printf("bound_range_exceeded");
	while(1);

}


void invalid_opcode(){
	printf("invalid_opcode");
	while(1);

}


void device_not_available(){
	printf("device_not_available");
	while(1);

}

void double_fault(){
	printf("double_fault");
	while(1);

}


void coprocessor_segment_overrun(){
	printf("coprocessor_segment_overrun");
	while(1);

}


void invalid_tss(){
	printf("invalid_tss");
	while(1);

}

void segment_not_present(){
	printf("segment_not_present");
	while(1);

}

void stack_segment(){
	printf("stack_segment_fault");
	while(1);

}

void general_protection(){
	printf("general_protection");
	while(1);

}

void page_fault(){
	printf("page_fault");
	while(1);

}

//vector 15 reserved for Intel use

void fpu_floating_point_exception(){
	printf("floating_point_exception");
	while(1);

}


void alignment_check(){
	printf("alignment_check");
	while(1);

}

void machine_check(){
	printf("machine_check");
	while(1);

}

void simd_floating_point_exception(){
	printf("divide_by_zero");
	while(1);

}

void sys_call(){
	printf("This is a system call");

}

void get_char(){
	printf("This is a keyboard interrupt");

}

void rtc_int(){
	printf("This is an RTC interrupt");

}






