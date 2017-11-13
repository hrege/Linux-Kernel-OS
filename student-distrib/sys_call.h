#include "types.h"

#ifndef _SYS_CALL_H
#define _SYS_CALL_H

#ifndef ASM

#define EXEC_IDENTITY     0x7F454C46	// "Magic Numbers" for an executable
#define EIP_SIZE			4
#define EIP_LOC				27
#define PROG_LOAD_LOC		0x08048000

#define USER_STACK_POINTER	0x083FFFFC
#define BOTTOM_KERN_PAGE_PTR  0x007FFFFC
#define EIGHT_KB			8192
#define EIGHT_MB			8388608
#define STACK_ROW_SIZE      4
#define USER_PROG_VM        128
#define PID_OFF             2
#define MAX_FILES           8

/* Declare variables */
extern uint32_t next_pid;

/* Declare functions */
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
