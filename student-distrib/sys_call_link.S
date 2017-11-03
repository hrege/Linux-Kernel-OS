# sys_call_link.S - Assembly linkage for system calls
# vim:ts=4 noexpandtab

#define ASM     1

#include "sys_call.h"
#include "sys_call_link.H"

# Assembly linkage functions for C handlers
# Generic Interface.  */

.global sys_call_asm
sys_call_asm:
pusha
pushl %edx
pushl %ecx
pushl %ebx
call *sys_jump($-1, %eax, 4)
addl $12, %esp
popa
leave
IRET




sys_jump:
.long sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close, sys_getargs, sys_vidmap, sys_set_handler, sys_sigreturn











