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
call (sys_jump, %eax)
pop 




sys_jump:
.long halt_asm, .long execute_asm, .long read_asm, .long write_asm, .long open_asm, .long close_asm, 
.long getargs_asm, .long vidmap_asm, .long set_handler_asm, .long sigreturn_asm




