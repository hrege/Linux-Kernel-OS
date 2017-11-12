/*
*	System call handlers
*	Authors: Sam and Jonathan
*/

#include "sys_call.h"
#include "filesystem.h"
#include "rtc.h"
#include "keyboard.h"
#include "paging.h"
#include "types.h"
#include "x86_desc.h"
#include "sys_call_link.H"

#define EXEC_IDENTITY     0x7F454C46	// "Magic Numbers" for an executable
#define EIP_SIZE			4
#define EIP_LOC				27
#define PROG_LOAD_LOC		0x08048000

#define USER_STACK_POINTER	0x083FFFFF

uint32_t next_pid;
file_operations_t stdin_ops = {terminal_open, terminal_read, NULL, terminal_close};
file_operations_t stdout_ops = {terminal_open, NULL, terminal_write, terminal_close};
file_operations_t regular_ops = {file_open, file_read, file_write, file_close};
file_operations_t directory_ops = {directory_open, directory_read, directory_write, directory_close};
file_operations_t rtc_ops = {rtc_open, rtc_read, rtc_write, rtc_close};


/*	get_first_fd
*		Description:  Local function to find first fd available in fd_array of pcb
*		Author: Sam
*		Inputs: None
*		Ouptuts: None
*		Returns: Number of first available fd;
*					-1 if fd_array is full
*/

int get_first_fd(){
	int i; // loop variable

	PCB_t* curr_pcb = (PCB_t*)((int32_t)tss.esp0 & 0xFFFFE000);

	/* start at 0 and check until you find one */
	for(i = 0; i < 8; i++){
		if(curr_pcb->file_array[i].flags == 0){
			curr_pcb->file_array[i].flags = 1;
			return i;
		}
	}
	return -1;
}

/*
*	sys_halt
*		Description: the system call to halt a process (the process called)
*		Author: Sam....
*		Input: ______
*		Output: ______
*		Returns: _____
*		Side effects: closes associated files ________
*/
extern uint32_t sys_halt(uint8_t status){
	int i; // loop variable
	PCB_t* curr_pcb = (PCB_t*)((int32_t)tss.esp0 & 0xFFFFE000);

	/*Close any files associated with this process*/
	for(i = 0; i < 8; i++){
		curr_pcb->file_array[i].file_operations.device_close(i);
	}

	return 0;
}


/*
*	sys_execute
*		Description: The execute system call to execute a new process
*		Author: Sam, Jonathan, Austin, Hershel
*		Inputs: command - a buffer containing the executable name and arguments
*					(**Arguments not yet supported for CP3)
*		Outputs: None
*		Returns: -1 if fails
*		Side effect: New process starts
*/
extern uint32_t sys_execute(const uint8_t* command){
	dentry_t exec;
	uint8_t* file_buffer;
	uint32_t* kern_stack_ptr;
	uint8_t* eip;
	int i;

	if(-1 == read_dentry_by_name(command, &exec)){
		return -1;
	}
	if(exec.file_type != REGULAR_FILE_TYPE){
		return -1;
	}
	/*Read executable into the file_buffer*/
	if(-1 == read_data(exec.inode_number, 0, file_buffer, filesystem.inode_start[exec.inode_number].length)){
		return -1;
	}

	if(  *((uint32_t *)file_buffer) != EXEC_IDENTITY){
		return -1;
	}


	/*Hershel sets up PCB using TSS stuff*/
	/*kernel stack pointer for process about to be executed*/
	kern_stack_ptr = (uint32_t*)(0x0800000 - 1 - (0x2000 * next_pid));
	PCB_t * exec_pcb = pcb_init(kern_stack_ptr, next_pid, (uint32_t *)(tss.esp0 & 0xFFFFE000));
	if(NULL == exec_pcb){
		return -1;

	}
	/*Austin's paging thing including flush TLB entry associated with 128 + offset MB virtual memory*/

	paging_switch(128, 4 * (exec_pcb->process_id + 2));


	/*Load Program */
	for(i = 0; i < filesystem.inode_start[exec.inode_number].length; i++){
		*((uint8_t*)(PROG_LOAD_LOC + i)) = file_buffer[i];
	}

	/*Load first instruction location into eip (reverse order since it's little-endian)*/
	for(i = 0; i < EIP_SIZE; i++){
		eip[i] = file_buffer[EIP_LOC - i];
	}


	user_prep(*((uint32_t*)eip), USER_STACK_POINTER);

	/* Set up stacks before IRET */
/*

	-Parse
	-Check if an executable
	-Set up Paging
	-User level program loader
	-Creae PCB for the program
		-assign pid based on global pid pointer
	-Context switch
*/
	return 0;
}

/* sys_read
*		Description: the read system call handler
*		Author: Sam
*		Input: fd (which file); buf (buffer to read into); nbytes(how much to read)
*		Ouputs: none
*		Returns: the return value from the file specific read handler
*		Side effect: calls the read handler based on file type
*/
extern uint32_t sys_read(uint32_t fd, void* buf, uint32_t nbytes){
	PCB_t* curr_pcb = (PCB_t*)((int32_t)tss.esp0 & 0xFFFFE000);

	return curr_pcb->file_array[fd].file_operations.device_read(fd, buf, nbytes);
}
/* sys_write
*		Description: the write system call handler
*		Author: Sam
*		Input: fd (which file); buf (buffer to write from); nbytes(how much to write)
*		Ouputs: none
*		Returns: the return value from the file specific write handler
*		Side effect: calls the write handler based on file type
*/
extern uint32_t sys_write(uint32_t fd, void* buf, uint32_t nbytes){
	PCB_t* curr_pcb = (PCB_t*)((int32_t)tss.esp0 & 0xFFFFE000);

	return curr_pcb->file_array[fd].file_operations.device_write(fd, buf, nbytes);
}
/* sys_open
*		Description: the open system call handler -- opens file
*		Author: Sam
*		Input: filename
*		Ouputs: none
*		Returns: -1 for fail; 0 for pass
*		Side effect: File is opened and has fd in the fd array
*/
extern uint32_t sys_open(const uint8_t* filename){
	int fd;
	/*Place holder*/
	PCB_t* curr_pcb = (PCB_t*)((int32_t)tss.esp0 & 0xFFFFE000);

	/*Read file by name*/
	dentry_t this_file;
	if(-1 == read_dentry_by_name(filename, &this_file)){
		return -1;
	}


	fd = get_first_fd(); //should get the first available fd and returns its index... -1 if full
	if(fd == -1){
		return -1;
	}
	/* Initialize correct FOP associations */
	switch(this_file.file_type) {
		case STD_IN_FILE_TYPE :
		curr_pcb->file_array[fd].file_operations = stdin_ops;

		case STD_OUT_FILE_TYPE :
		curr_pcb->file_array[fd].file_operations = stdout_ops;
		//Only need to open terminal once and stdin will always be called prior terminal_open(1);
		case REGULAR_FILE_TYPE :
		curr_pcb->file_array[fd].file_operations = regular_ops;

		case DIRECTORY_FILE_TYPE :
		curr_pcb->file_array[fd].file_operations = directory_ops;

		case RTC_FILE_TYPE :
		curr_pcb->file_array[fd].file_operations = rtc_ops;

		default:
		return -1;

	}

	/* Open the file*/
	curr_pcb->file_array[fd].file_operations.device_open(filename);

	return 0;
}

/* sys_close
*		Description: the close system call handler -- closes file
*		Author: Sam
*		Input: fd number
*		Ouputs: none
*		Returns: 0 for pass
*		Side effect: File ic closed and entry in fd freed
*/
extern uint32_t sys_close(uint32_t fd){
	PCB_t* curr_pcb = (PCB_t*)((int32_t)tss.esp0 & 0xFFFFE000);
	//SHOULD WE CHECK IF THE FD IS VALID??????
	curr_pcb->file_array[fd].flags = 0;
	curr_pcb->file_array[fd].file_operations.device_close(fd);
	return 0;
}


/*    BELOW NOT FOR CP3   */

extern uint32_t sys_getargs(uint8_t* buf, uint32_t nbytes){
	return 0;

}
extern uint32_t sys_vidmap(uint8_t** screen_start){
	return 0;

}
extern uint32_t sys_set_handler(uint32_t signum, void* handler_address){
	return 0;

}
extern uint32_t sys_sigreturn(void){
	return 0;

}
