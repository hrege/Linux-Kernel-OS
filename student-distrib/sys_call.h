#include "types.h"

#ifndef _SYS_CALL_H
#define _SYS_CALL_H

#ifndef ASM

/* Declare variables */
#define EXEC_IDENTITY          0x7F454C46	// "Magic Numbers" for an executable
#define EIP_SIZE			         4
#define EIP_LOC				         27
#define PROG_LOAD_LOC		       0x08048000

#define USER_STACK_POINTER	   0x083FFFFC
#define BOTTOM_KERN_PAGE_PTR   0x007FFFFC
#define EIGHT_KB			         8192
#define EIGHT_MB			         8388608
#define STACK_ROW_SIZE         4
#define USER_PROG_VM           128
#define PID_OFF                2
#define MAX_FILES              8
#define MAX_PROCESS			       6  //current support limit
#define MAX_PID     		       6

#define _128_MB                0x8000000
#define _132_MB				         0x8400000
#define _8_MB				           0x800000
#define _4_MB				           0x400000
#define REGULAR_FILE_TYPE      2
#define DIRECTORY_FILE_TYPE    1
#define RTC_FILE_TYPE          0
#define VID_MEM_VIRT_MB        132
#define VID_MEM_PHYS_MB        0
#define FILE_BUFFER_SIZE	   30

extern volatile int echo;
extern int exe_flag;
extern uint32_t pid_bitmap[MAX_PID];
extern int shell_2;
extern int shell_3;

int get_first_pid();

/* extern function to check pid */
extern int check_pid(uint8_t pid);

/* extern function to check if terminal is running a non-shell process */
extern int non_shell(uint8_t term);

/* Declare functions */
int32_t sys_halt(uint8_t status);
int32_t sys_execute(const uint8_t* command);
int32_t sys_read(int32_t fd, void* buf, int32_t nbytes);
int32_t sys_write(int32_t fd, void* buf, int32_t nbytes);
int32_t sys_open(const uint8_t* filename);
int32_t sys_close(int32_t fd);
int32_t sys_getargs(uint8_t* buf, int32_t nbytes);
int32_t sys_vidmap(uint8_t** screen_start);
int32_t sys_set_handler(int32_t signum, void* handler_address);
int32_t sys_sigreturn(void);
int32_t blank_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t blank_read(int32_t fd, void* buf, int32_t nbytes);

#endif
#endif
