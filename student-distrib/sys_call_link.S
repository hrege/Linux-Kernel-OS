# sys_call_link.S - Assembly linkage for system calls
# vim:ts=4 noexpandtab

#define ASM     1

#include "sys_call.h"
#include "sys_call_link.H"
#include "x86_desc.h"

# Assembly linkage functions for C handlers
# Generic Interface.  */

KERNEL_CS_A:  .long 0x10
KERNEL_DS_A:  .long 0x18
USER_CS_A:    .long 0x23
USER_DS_A:    .long 0x2B
ZERO:         .long 0x0

.global sys_call_asm, user_prep #, execute_comeback
sys_call_asm:
pushl %ebp
movl %esp, %ebp
cmpl $0, %eax
jle bad_sys_call

cmpl $11, %eax
jge bad_sys_call

#save registers
pushl %edi
pushl %esi
pushl %ebp
pushl %ebx
pushl %edx
pushl %ecx
#push args
pushl %edx
pushl %ecx
pushl %ebx
movl $-4, %ebx #to account for indexing of the jump table
sti
call *sys_jump(%ebx, %eax, 4)

done:
#for 3 args
addl $12, %esp
#restore registers
popl %ecx
popl %edx
popl %ebx
popl %ebp
popl %esi
popl %edi
leave
iret


bad_sys_call:
movl $-1, %eax
leave
ret

sys_jump:
.long sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close, sys_getargs, sys_vidmap, sys_set_handler, sys_sigreturn

user_prep:
cli

                        #save esp/ebp here

movl USER_DS_A, %edx    #user data segment -> DS Register
movl %edx, %ds
movl %edx, %es
movl %edx, %fs
movl %edx, %gs
pushl USER_DS_A       #user SS
pushl 12(%esp)	    #user ESP
pushfl		        #get EFLAGS and enable IF (then push to stack)
popl %edx
orl $0x200, %edx
pushl %edx
pushl USER_CS_A       #user CS
pushl 20(%esp)      #user EIP
IRET
#execute_comeback:
#ret
