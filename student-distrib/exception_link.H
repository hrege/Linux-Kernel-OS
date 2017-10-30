/*
*  Author(); Jonathan - Group 31 victoriOS Secret
*/

#ifndef EXCEPTION_LINK_H
#define EXCEPTION_LINK_H

#ifndef ASM
//Divide by zero exception
extern void divide_by_zero_asm();

// vector #1 reserved for Intel debug use
extern void debug_asm();

//Non-maskable interrupt exception
extern void nmi_interrupt_asm();

//breakpoint exception
extern void breakpoint_asm();

//overflow exception
extern void overflow_asm();

//range exceeded exception
extern void bound_range_exceeded_asm();

//invalide opcode exception
extern void invalid_opcode_asm();

//device not available exception
extern void device_not_available_asm();

//double fault exception
extern void double_fault_asm();

//segment overrun exception
extern void coprocessor_segment_overrun_asm();

//invaid tss exception
extern void invalid_tss_asm();

//non-present segment exception
extern void segment_not_present_asm();

//stack segment exception
extern void stack_segment_asm();

//general protection exception
extern void general_protection_asm();

//page fault exception
extern void page_fault_asm();

//vector 15 reserved for Intel use
//Except we are using at assertion fail as specified by given code
extern void assertion_fail_asm();

//fpu floating point exception
extern void fpu_floating_point_exception_asm();

//alignment check exception
extern void alignment_check_asm();

//machine check exception
extern void machine_check_asm();

//simd floating pt. exception
extern void simd_floating_point_exception_asm();


#endif
#endif
