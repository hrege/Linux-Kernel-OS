/*
*  Author: Jonathan - Group 31 victoriOS Secret
*
*
*	exceptions file is for the exception handlers
*/

#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H

#include "types.h"

#define HALT_STATUS		256

#ifndef ASM
/* divide by zero handler */
void divide_by_zero();

/* vector # 1 reserved for Intel debug use */
extern void debug();

/* non-maskable interrupt exception handler */
extern void nmi_interrupt();

/* breakpoint exception handler */
extern void breakpoint();

/* overflow exception handler */
extern void overflow();

/* range exceeded exception handler */
extern void bound_range_exceeded();

/* invalid opcode exception handler */
extern void invalid_opcode();

/* unavailable device exception handler */
extern void device_not_available();

/* double fault exception handler */
extern void double_fault();

/* segment overrun exception handler */
extern void coprocessor_segment_overrun();

/* invalid tss exception handler */
extern void invalid_tss();

/* segment not pressed exception handler */
extern void segment_not_present();

/* stack segment exception handler */
extern void stack_segment();

/* general protection exception handler */
extern void general_protection();

/* page fault exception handler */
extern void page_fault();

/* vector 15 reserved for Intel use
 	we use to handle assertion fail exception handler */
extern void assertion_fail();

/* fpu floating point exception handler */
extern void fpu_floating_point_exception();

/* alignment exception handler */
extern void alignment_check();

/* machine check exception handler */
extern void machine_check();

/* simd floating point exception handler */
extern void simd_floating_point_exception();


#endif
#endif

