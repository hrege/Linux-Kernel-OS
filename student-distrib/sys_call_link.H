/*
*  Authors: Sam and Jonathan
*/

#ifndef SYS_CALL_LINK_H
#define SYS_CALL_LINK_H

#ifndef ASM

//Divide by zero exception
extern void sys_call();
extern void sys_call_asm();
extern void user_prep(uint32_t eip, uint32_t esp);

#endif
#endif
