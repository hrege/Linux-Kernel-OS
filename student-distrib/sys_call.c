/*
*	System call handlers
*	Authors: Sam and Jonathan
*/

#include "sys_call.h"
#include "filesystem.h"
#include "rtc.h"
#include "keyboard.h"

#define EXEC_IDENTITY     0x7F454C46
/*Local function to find first fd available in fd_array of pcb*/
int get_first_fd(){
	int i; // loop variable
	/*tss placeholder until we figure out what it should be*/
	PCB_t* curr_pcb = tss.esp0 & 0xFFFFE000; 

	
	for(i = 0; i < 8; i++){
		if(curr_pcb->file_array[i].flags == 0){
			curr_pcb->file_array[i].flags = 1;
			return i;

		}
	}

	return -1;
}

extern int32_t sys_halt(uint8_t status){


}
extern int32_t sys_execute(const uint8_t* command){
	dentry_t exec;
	char* file_buffer;

	if(-1 == read_dentry_by_name(command, &exec)){
		return -1;
	}
	if(exec.file_type != REGULAR_FILE_TYPE){
		return -1;
	}
	/*Read executable into the file_buffer*/
	if(-1 == read_data(exec.inode_number, 0, file_buffer, filesystem.inode_start[exec.inode_number])){
		return -1; 
	}

	if(  *((uint32_t *)file_buffer) != EXEC_IDENTITY){
			return -1;
	}

	/*Austin's paging thing including flush TLB entry associated with 128 + offset MB virtual memory*/

	/*Load Program */


	/*Hershel sets up PCB using TSS stuff*/ 



/*
	
	-Parse
	-Check if an executable
	-Set up Paging
	-User level program loader
	-Creae PCB for the program
	-assign pid based on global pid pointer
	-Context switch
*/

}
extern int32_t sys_read(int32_t fd, void* buf, int32_t nbytes){
	PCB_t* curr_pcb = tss.esp0 & 0xFFFFE000;

	return curr_pcb->file_array[fd].file_operations[1](fd, buf, nbytes);

}
extern int32_t sys_write(int32_t fd, void* buf, int32_t nbytes){
	PCB_t* curr_pcb = tss.esp0 & 0xFFFFE000;

	return curr_pcb->file_array[fd].file_operations[2](fd, buf, nbytes);
}
extern int32_t sys_open(const uint8_t* filename){
	int fd;
	/*Place holder*/
	PCB_t* curr_pcb = tss.esp0 & 0xFFFFE000;

	/*Read file by name*/
	dentry_t this_file;
	if(-1 == read_dentry_by_name(filename, &this_file)){
		return -1;
	}


	fd = get_first_fd(); //should get the first available fd and returns its index... -1 if full
	if(fd == -1){
		return -1;
	}
	/*Switch by file type and associate correct ops in file_array[fd].file_operations*/
	switch(this_file.file_type){
		case STD_IN_FILE_TYPE:
		curr_pcb->file_array[fd].file_operations = {&terminal_open, &terminal_read, NULL, &terminal_close};

		case STD_OUT_FILE_TYPE:
		curr_pcb->file_array[fd].file_operations = {&terminal_open, NULL, &terminal_write, &terminal_close};
		//Only need to open terminal once and stdin will always be called prior terminal_open(1);
		case REGULAR_FILE_TYPE:
		curr_pcb->file_array[fd].file_operations = {&file_open, &file_read, &file_write, &file_close};

		case DIRECTORY_FILE_TYPE:
		curr_pcb->file_array[fd].file_operations = {&directory_open, &directory_read, &directory_write, &directory_close};

		case RTC_FILE_TYPE:
		curr_pcb->file_array[fd].file_operations = {&rtc_open, &rtc_read, &rtc_write, &rtc_close};
		default:
		return -1;


	}

	curr_pcb->file_array[fd].file_operations[0](filename);
	
	
	return 0;
	
	


}
extern int32_t sys_close(int32_t fd){
	PCB_t* curr_pcb = tss.esp0 & 0xFFFFE000;

	curr_pcb->file_array[i].flags = 0;
	curr_pcb->file_array[fd].file_operations[3](fd);

}


/*    BELOW NOT FOR CP3   */

extern int32_t sys_getargs(uint8_t* buf, int32_t nbytes){
	return 0;

}
extern int32_t sys_vidmap(uint8_t** screen_start){
	return 0;

}
extern int32_t sys_set_handler(int32_t signum, void* handler_address){
	return 0;

}
extern int32_t sys_sigreturn(void){
	return 0;

}

int get_first_fd(){


}


