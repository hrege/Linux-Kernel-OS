/*
*	System call handlers
*	Authors: Sam and Jonathan
*/

#include "sys_call.h"
#include "filesystem.h"




extern int32_t sys_halt(uint8_t status){


}
extern int32_t sys_execute(const uint8_t* command){
/*
	-Parse
	-Check if an executable
	-Set up Paging
	-User level program loader
	-Creae PCB for the program
	-COntext switch
/*

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
	/* Do the open function for that file? */

	get_first_fd() //should get the first available fd and returns its index... -1 if full
	


}
extern int32_t sys_close(int32_t fd){


}


/*    BELOW NOT FOR CP3   */

extern int32_t sys_getargs(uint8_t* buf, int32_t nbytes){


}
extern int32_t sys_vidmap(uint8_t** screen_start){


}
extern int32_t sys_set_handler(int32_t signum, void* handler_address){


}
extern int32_t sys_sigreturn(void){


}




