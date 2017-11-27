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
#include "lib.h"


uint32_t pid_bitmap[MAX_PID];
file_operations_t stdin_ops;
// stdin_ops.device_open = terminal_open;
// stdin_ops.device_close = terminal_close;
// stdin_ops.device_read = terminal_read;
// stdin_ops.device_write = blank_write;


file_operations_t stdout_ops;
// stdout_ops.device_open = terminal_open;
// stdout_ops.device_read = blank_read;
// stdout_ops.device_write = terminal_write;
// stdout_ops.device_close = terminal_close;


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
	for(i = 0; i < MAX_ACTIVE_FILES; i++){
		if(curr_pcb->file_array[i].flags == 0){
			curr_pcb->file_array[i].flags = 1;
			return i;
		}
	}
	return -1;
}

/*	get_first_pid
*		Description:  Local function to find first pid available in pid_bitmap located in kernel memory
*		Author: Sam
*		Inputs: None
*		Ouptuts: None
*		Returns: Number of first available pid;
*					-1 if pid_bitmap is full
*/

int get_first_pid(){
	int i; // loop variable

	/* start at 0 and check until you find one */
	for(i = 0; i < MAX_PID; i++){
		if(pid_bitmap[i] == 0){
			pid_bitmap[i] = 1;
			return i;
		}
	}
	return -1;
}

/*
*	sys_halt
*		Description: the system call to halt a process (the process called)
*		Author: Sam
*		Input: status - status of execution (256 if halted by exception)
*		Outputs: Halts process
*		Returns: status (returned to execute)
*		Side effects: closes associated files
*/
extern uint32_t sys_halt(uint8_t status){
	int i; // loop variable
	/* Halt outline compilation from Office Hrs. and ULK
		-Account for returning from the only running process? Depends on how we relaunch shell ... loop in kernel? or relaunch here?
		-Make process # available
		-Set current process back to parent
		-If our pcb has a child counter decrement that (we don't and its a design choice - not required)
		-Load the parent page directory (going to need assembly here - can access pcb struct elements by by casting their pointers to uint32 for use in assembly)
		-Set kerel stack bottom and tss.esp0 back to parent (should be from PCB?)
		-reset stack pointers (esp and ebp)
		-depending on how you set up stack you may need to pop off one or two things here to remove useless stuff
		-leave
		-return
	*/

	PCB_t* curr_pcb = (PCB_t*)((int32_t)tss.esp0 & 0xFFFFE000);

	if(pid_bitmap[curr_pcb->process_id] == 0){
		return -1;
	}

	pid_bitmap[curr_pcb->process_id] = 0;

	if(curr_pcb->process_id == 0){
		uint8_t* ptr = (uint8_t*)("shell");
		sys_execute(ptr);
	}


	paging_switch(128, 4 * (curr_pcb->parent_process->process_id + 2));
	/*Close any files associated with this process*/
	for(i = 0; i < MAX_ACTIVE_FILES; i++){
		if(curr_pcb->file_array[i].flags == 1) {
			curr_pcb->file_array[i].file_operations.device_close(i);
		}
	}
	/*Need to restore stack frame stored in pcb*/
	tss.esp0 = (uint32_t)(EIGHT_MB - STACK_ROW_SIZE - (EIGHT_KB * curr_pcb->parent_process->process_id));
	tss.ss0 = KERNEL_DS;
	/* Restore ESP from calling Execute function. This works
		 and sends program to sys_execute
	 */
  asm("movl %0, %%esp;"
	  "movl %1, %%ebp;"
	  "jmp execute_comeback;"
  	  :
	  : "m"(curr_pcb->parent_process->kern_esp), "m"(curr_pcb->parent_process->kern_ebp)
	  : "memory");
  	
	return 0;
}


/*
*	sys_execute
*		Description: The execute system call to execute a new process
*		Author: Sam, Jonathan, Austin, Hershel
*		Inputs: command - a buffer containing the executable name and arguments
*		Outputs: None
*		Returns: 0 if successful, -1 if fails
*		Side effect: New process starts
*/
extern uint32_t sys_execute(const uint8_t* command){
	uint32_t mag_num;
	dentry_t exec;
	uint8_t file_buffer[30];
	uint8_t exe_name[FILE_NAME_SIZE];
	uint8_t exe_len = 0;
	uint8_t temp_arg_buf[128];
	uint8_t arg_len_count = 0;
	uint32_t* kern_stack_ptr;
	uint32_t eip;
	uint32_t next_pid;
	int i;

	//check valid PID and command buffer
	next_pid = get_first_pid();
	if(next_pid == -1 || command == NULL){
		return -1;
	}

	clear(); //WHY??
	int args = 0; //indicator of whether we are reading arguments or not
	int leading_space = 1;
	/*  PARSE THE COMMAND FOR EXECULTABLE AND ARGS */
	for(i=0; command[i] != '\0'; i++){
		//if this is the executabke
		if(!args){
			//if we reach the end of the executable
			if(command[i] == ' '){
				exe_name[i] = '\0';
				exe_len = i + 1;
				args = 1;
			}
			//if the input it too long
			else if(i>=32){
				return -1;
			}
			//if its all good copy the executable
			else{
				exe_name[i] = command[i];
			}
		}
		//if it is not the executable copy into args buf
		else{
			//check if leading space
			if(!(leading_space && command[i]==' ')){
				leading_space = 0;
				temp_arg_buf[arg_len_count] = command[i];
				arg_len_count++;
			}
		}
	}
	//account for a null terminated executable name (no space)
	if(!args){
		exe_name[i]='\0';
	}
	//null terminate the arg buf
	temp_arg_buf[i-exe_len] = '\0';
	arg_len_count++;


	if(-1 == read_dentry_by_name(exe_name, &exec)){
		return -1;
	}
	if(exec.file_type != REGULAR_FILE_TYPE){
		return -1;
	}

	inode_t* this_inode;
	this_inode = (inode_t*)((uint8_t*)filesystem.inode_start + (exec.inode_number * BLOCK_SIZE));
	this_inode->length = *((uint32_t*)this_inode);

	/*Read executable into the file_buffer*/
	if(-1 == read_data(exec.inode_number, 0, file_buffer, 30)){ //filesystem.inode_start[exec.inode_number].length
		return -1;
	}

	mag_num = (((uint32_t)(file_buffer[0]) << 24) | ((uint32_t)(file_buffer[1]) << 16) | ((uint32_t)(file_buffer[2]) << 8) | (uint32_t)(file_buffer[3]));
	if(mag_num != EXEC_IDENTITY){
		return -1;
	}


	/*Hershel sets up PCB using TSS stuff*/
	/*kernel stack pointer for process about to be executed*/
	kern_stack_ptr = (uint32_t*)(EIGHT_MB - STACK_ROW_SIZE - (EIGHT_KB * next_pid));
	PCB_t * exec_pcb = pcb_init(kern_stack_ptr, next_pid, (PCB_t *)(tss.esp0 & 0xFFFFE000));
	if(NULL == exec_pcb){
		return -1;
	}
	/* Put args into PCB */
	exec_pcb->arg_len = arg_len_count;
	strcpy((int8_t*)exec_pcb->arguments,(const int8_t*)temp_arg_buf);

	/* Set up standard IN/OUT (should we just call open maybe...)*/
	exec_pcb->file_array[0].file_operations.device_open = terminal_open;
	exec_pcb->file_array[0].file_operations.device_close = terminal_close;
	exec_pcb->file_array[0].file_operations.device_read = terminal_read;
	exec_pcb->file_array[0].file_operations.device_write = blank_write;

	exec_pcb->file_array[1].file_operations.device_open = terminal_open;
	exec_pcb->file_array[1].file_operations.device_close = terminal_close;
	exec_pcb->file_array[1].file_operations.device_read = blank_read;
	exec_pcb->file_array[1].file_operations.device_write = terminal_write;

	exec_pcb->file_array[0].flags = 1;
	exec_pcb->file_array[1].flags = 1;

	/*Austin's paging thing including flush TLB entry associated with 128 + offset MB virtual memory*/
	paging_switch(USER_PROG_VM, STACK_ROW_SIZE * (exec_pcb->process_id + PID_OFF));


	/*Load Program */
	if(-1 == read_data(exec.inode_number, 0, (uint8_t*)PROG_LOAD_LOC, this_inode->length)){
		return -1;
	}
	// for(i = 0; i < this_inode->length; i++){
	// 	*((uint8_t*)(PROG_LOAD_LOC + i)) = file_buffer[i];
	// }

	/*Load first instruction location into eip (reverse order since it's little-endian)*/
	eip = ((uint32_t)(file_buffer[EIP_LOC]) << 24) | ((uint32_t)(file_buffer[EIP_LOC - 1]) << 16) | ((uint32_t)(file_buffer[EIP_LOC - 2]) << 8) | ((uint32_t)(file_buffer[EIP_LOC - 3]));

  asm ("movl %%esp, %0;"
			"movl %%ebp, %1;"
			: "=m"(exec_pcb->kern_esp), "=m"(exec_pcb->kern_ebp)
			:
			: "memory"
			);
  	//kern_stack_ptr = exec_pcb->kern_esp;
	/* Set TSS values (more for safety than necessity) */
	tss.esp0 = (uint32_t)kern_stack_ptr;
	tss.ss0 = KERNEL_DS;

	/* Set up stacks before IRET */
	user_prep(eip, USER_STACK_POINTER);
	asm ("execute_comeback:"
			"LEAVE;"
			"RET;"
	);
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
	if(fd > MAX_FILES || fd < 0) {
		return -1;
	}

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
	if(fd > MAX_FILES || fd < 0) {
		return -1;
	}

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
	if(fd > MAX_FILES || fd < 0) {
		return -1;
	}

	PCB_t* curr_pcb = (PCB_t*)((int32_t)tss.esp0 & 0xFFFFE000);

	curr_pcb->file_array[fd].flags = 0;
	curr_pcb->file_array[fd].file_operations.device_close(fd);
	return 0;
}


/*    BELOW NEW FOR CP4  */

/*
*	sys_getargs
*		Author: Jonathan
*		Description: returns the command line args for a process stored in task data
*		Inputs: The buffer the program wants the arguments in and the lenght of it
*		Returns: -1 for fail or 0 for pass
*		Side-effect: If succeeds then the buffer has the null terminated args in it
*/
extern uint32_t sys_getargs(uint8_t* buf, uint32_t nbytes){
	//Input check
	if(buf == NULL || nbytes == 0){
		return -1;
	}
	//get the pcb
	PCB_t* curr_pcb = (PCB_t*)((int32_t)tss.esp0 & 0xFFFFE000);
	//check length
	if(curr_pcb->arg_len > nbytes){
		return -1;
	}
	//make copy
	strcpy((int8_t*)buf, (const int8_t*)curr_pcb->arguments);
	return 0;
}

/*
*	sys_vidmap
*		Author: Jomathan
*		Description: Maps the text mode video mem into user space at preset virtual addr. 
*		Inputs: Pointer to where to put the pointer
*		Return: -1 for fail ... virtual address (const 132MB) upon success
*		Side_effects: Video Mem mapped to 132MB in virtual mem
*/
extern uint32_t sys_vidmap(uint8_t** screen_start){
	//check for non-NULL
	if(screen_start == NULL){
		return -1;
	}
	//figure out where user page is by process number

//Checking valid location not right yet

	// PCB_t* curr_pcb = (PCB_t*)((int32_t)tss.esp0 & 0xFFFFE000);
	// uint32_t id = curr_pcb->process_id;
	// //make sure in that user page
	// if((uint32_t)screen_start < (_8_MB + (_4_MB * id)) || (uint32_t)screen_start > (_8_MB + (_4_MB * (id+1)))){
	// 	return -1;
	// }

	/*Set screen start pointer to the virtual location
		we are using the 4mb starting at 132 MB because that is,
		at the time this is written, the first 4mb available in memory */
	*screen_start = (uint8_t *)_132_MB;

	/*map based on video number (this depends on whether we have implemented multiple terminals?)
	*Call our paging mapping function ... input 1 is virtual location; 2 is physical location
	*Virtual Location is:  132 MB
	*Physical Location is:   Video Mem at 0xB8000
	*/
	paging_switch((uint32_t)_132_MB,(uint32_t)VIDEO_LOC);
	
	//return the virtual location
	return _132_MB;

}

/*
*	sys_set_handler
*		Not supported. Returns Fail
*/
extern uint32_t sys_set_handler(uint32_t signum, void* handler_address){
	return 1;
}
/*
*	sys_set_handler
*		Not supported. Returns Fail
*/
extern uint32_t sys_sigreturn(void){
	return -1;
}


/* Below are place holders for calls table */
extern int32_t blank_write(int32_t fd, const void* buf, int32_t nbytes) {
	return 0;
}
extern int32_t blank_read(int32_t fd, void* buf, int32_t nbytes) {
	return 0;
}
