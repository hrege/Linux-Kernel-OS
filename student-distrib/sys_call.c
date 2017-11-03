/*
*	System call handlers
*	Authors: Sam and Jonathan
*/

#include "sys_call.h"
#include "filesystem.h"




extern int32_t sys_halt(uint8_t status){


}
extern int32_t sys_execute(const uint8_t* command){


}
extern int32_t sys_read(int32_t fd, void* buf, int32_t nbytes){


}
extern int32_t sys_write(int32_t fd, void* buf, int32_t nbytes){


}
extern int32_t sys_open(const uint8_t* filename){
	/*Read file by name*/
	/*Select fd to be first available in fd_array located in the PCB struct*/
	/*Switch by file type*/
	/*associate correct ops in fd_array[fd].ops_table*/

}
extern int32_t sys_close(int32_t fd){


}
extern int32_t sys_getargs(uint8_t* buf, int32_t nbytes){


}
extern int32_t sys_vidmap(uint8_t** screen_start){


}
extern int32_t sys_set_handler(int32_t signum, void* handler_address){


}
extern int32_t sys_sigreturn(void){


}




