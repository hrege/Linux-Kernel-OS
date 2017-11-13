#include "types.h"

#ifndef _SYS_CALL_H
#define _SYS_CALL_H

#ifndef ASM

extern uint32_t next_pid;

extern uint32_t sys_halt(uint8_t status);
extern uint32_t sys_execute(const uint8_t* command);
extern uint32_t sys_read(uint32_t fd, void* buf, uint32_t nbytes);
extern uint32_t sys_write(uint32_t fd, void* buf, uint32_t nbytes);
extern uint32_t sys_open(const uint8_t* filename);
extern uint32_t sys_close(uint32_t fd);
extern uint32_t sys_getargs(uint8_t* buf, uint32_t nbytes);
extern uint32_t sys_vidmap(uint8_t** screen_start);
extern uint32_t sys_set_handler(uint32_t signum, void* handler_address);
extern uint32_t sys_sigreturn(void);
extern int32_t blank_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t blank_read(int32_t fd, void* buf, int32_t nbytes);

#endif
#endif
