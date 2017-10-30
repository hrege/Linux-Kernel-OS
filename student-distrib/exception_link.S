# exception_link.S - Assembly linkage for exception handlers
# vim:ts=4 noexpandtab

#define ASM     1

#include "exceptions.h"
#include "exception_link.H"

# Assembly linkage functions for C handlers
# Interface is same for all of them. See divide by zero for example */

#
#	divide_by_zero()
#		Author: Sam & Jonathan
#		Description: This is an assembly linkage between to a handler from a divide
#						by zero excpetion coming through the idt
#		inputs: none
#		outputs: none
#		side effects: calls divide_by_zero function (from exception.c)
#
.global divide_by_zero_asm
divide_by_zero_asm:
	pusha
	call divide_by_zero
	popa
	leave
	IRET


# vector # 1 reserved for Intel use
.global debug_asm
debug_asm:
	pusha
	call debug
	popa
	leave
	IRET

#Non-maskable interrupt exception
.global nmi_interrupt_asm
nmi_interrupt_asm:
	pusha
	call nmi_interrupt
	popa
	leave
	IRET

#breakpoint exception
.global breakpoint_asm
breakpoint_asm:
	pusha
	call breakpoint
	popa
	leave
	IRET

#overflow exception
.global overflow_asm
overflow_asm:
	pusha
	call overflow
	popa
	leave
	IRET

#range exceeded exception
.global bound_range_exceeded_asm
bound_range_exceeded_asm:
	pusha
	call bound_range_exceeded
	popa
	leave
	IRET


#invalide opcode exception
.global invalid_opcode_asm
invalid_opcode_asm:
	pusha
	call invalid_opcode
	popa
	leave
	IRET

#device not available exception
.global device_not_available_asm
device_not_available_asm:
	pusha
	call device_not_available
	popa
	leave
	IRET

#double fault exception
.global double_fault_asm
double_fault_asm:
	pusha
	call double_fault
	popa
	leave
	IRET

#segment overrun exception
.global coprocessor_segment_overrun_asm
coprocessor_segment_overrun_asm:
	pusha
	call coprocessor_segment_overrun
	popa
	leave
	IRET

#invaid tss exception
.global invalid_tss_asm
invalid_tss_asm:
	pusha
	call invalid_tss
	popa
	leave
	IRET

#non-present segment exception
.global segment_not_present_asm
segment_not_present_asm:
	pusha
	call segment_not_present
	popa
	leave
	IRET

#stack segment exception
.global stack_segment_asm
stack_segment_asm:
	pusha
	call stack_segment
	popa
	leave
	IRET

#general protection exception
.global general_protection_asm
general_protection_asm:
	pusha
	call general_protection
	popa
	leave
	IRET

#page fault exception
.global page_fault_asm
page_fault_asm:
	pusha
	call page_fault
	popa
	leave
	IRET

#vector 15 reserved for Intel use
#Except we are using at assertion fail as specified by given code
.global assertion_fail_asm
assertion_fail_asm:
	pusha
	call assertion_fail
	popa
	leave
	IRET

#fpu floating point exception
.global fpu_floating_point_exception_asm
fpu_floating_point_exception_asm:
	pusha
	call fpu_floating_point_exception
	popa
	leave
	IRET

#alignment check exception
.global alignment_check_asm
alignment_check_asm:
	pusha
	call alignment_check
	popa
	leave
	IRET

#machine check exception
.global machine_check_asm
machine_check_asm:
	pusha
	call machine_check
	popa
	leave
	IRET

#simd floating pt. exception
.global simd_floating_point_exception_asm
simd_floating_point_exception_asm:
	pusha
	call simd_floating_point_exception
	popa
	leave
	IRET

