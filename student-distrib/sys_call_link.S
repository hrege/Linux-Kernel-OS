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

.global sys_call_asm, user_prep
sys_call_asm:
pushl %ebp
movl %esp, %ebp
cmpl $0, %eax
jle bad_sys_call

cmpl $11, %eax
jge bad_sys_call

pusha
pushl %edx
pushl %ecx
pushl %ebx
movl $-4, %ebx
sti
jmp *sys_jump(%ebx, %eax, 4)

done:
addl $12, %esp
popa
leave
iret


bad_sys_call:
movl $-1, %eax
leave
ret

sys_jump:
.long sys_halt_link, sys_execute_link, sys_read_link, sys_write_link, sys_open_link, sys_close_link, sys_getargs_link, sys_vidmap_link, sys_set_handler_link, sys_sigreturn_link



sys_halt_link:
call sys_halt
jmp done

sys_execute_link:
call sys_execute
jmp done

sys_read_link:
call sys_read
jmp done

sys_write_link:
call sys_write
jmp done

sys_open_link:
call sys_open
jmp done

sys_close_link:
call sys_close
jmp done

sys_getargs_link:
call sys_getargs
jmp done

sys_vidmap_link:
call sys_vidmap
jmp done

sys_set_handler_link:
call sys_set_handler
jmp done

sys_sigreturn_link:
call sys_sigreturn

jmp done


user_prep:
cli
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
jmp done

