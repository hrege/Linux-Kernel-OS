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
file_operations_t stdin_ops = {terminal_open, terminal_read, blank_write, terminal_close};
file_operations_t stdout_ops = {terminal_open, blank_read, terminal_write, terminal_close};
file_operations_t regular_ops = {file_open, file_read, file_write, file_close};
file_operations_t directory_ops = {directory_open, directory_read, directory_write, directory_close};
file_operations_t rtc_ops = {rtc_open, rtc_read, rtc_write, rtc_close};
int call_from_kern = 0;

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
int32_t sys_halt(uint8_t status){
	int i; // loop variable

	PCB_t* curr_pcb = (PCB_t*)((int32_t)tss.esp0 & 0xFFFFE000);

	if(pid_bitmap[curr_pcb->process_id] == 0){
		return -1;
	}

	pid_bitmap[curr_pcb->process_id] = 0;

	if(curr_pcb->process_id == 0){
		clear();
		uint8_t* ptr = (uint8_t*)("shell");
		sys_execute(ptr);
	}

	paging_switch(128, 4 * (curr_pcb->parent_process->process_id + 2));
	/*Close any files associated with this process*/
	for(i = 0; i < MAX_ACTIVE_FILES; i++){
		if(curr_pcb->file_array[i].flags == 1) {
			call_from_kern = 1;
			curr_pcb->file_array[i].file_operations.device_close(i);
		}
	}
	call_from_kern = 0;
	/*Need to restore stack frame stored in pcb*/
	tss.esp0 = (uint32_t)(EIGHT_MB - STACK_ROW_SIZE - (EIGHT_KB * curr_pcb->parent_process->process_id));
	tss.ss0 = KERNEL_DS;
	/* Restore ESP from calling Execute function. This works
		 and sends program to sys_execute
	 */

  asm volatile("movl %0, %%eax;"
		"movl %1, %%esp;"
	  "movl %2, %%ebp;"
	  "leave;"
		"ret;"
  	:
	  : "r"((uint32_t)status), "m"(curr_pcb->kern_esp), "m"(curr_pcb->kern_ebp)
	  : "eax"
		);

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
int32_t sys_execute(const uint8_t* command){
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
		printf("Maximum number of processes running!\n");
		return 0;
	}

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

	/* Check if file exists in directory and if file type is regular. */
	if(-1 == read_dentry_by_name(exe_name, &exec)){
		return -1;
	}
	if(exec.file_type != REGULAR_FILE_TYPE){
		return -1;
	}

	/* Calculate length of file being read. */
	inode_t* this_inode;
	this_inode = (inode_t*)((uint8_t*)filesystem.inode_start + (exec.inode_number * BLOCK_SIZE));
	this_inode->length = *((uint32_t*)this_inode);

	/* Read executable information into the file_buffer. */
	if(-1 == read_data(exec.inode_number, 0, file_buffer, 30)) {
		return -1;
	}

	/* Determine if file is executable by comparing it to pre-determined executable hex value. */
	mag_num = (((uint32_t)(file_buffer[0]) << 24) | ((uint32_t)(file_buffer[1]) << 16) | ((uint32_t)(file_buffer[2]) << 8) | (uint32_t)(file_buffer[3]));
	if(mag_num != EXEC_IDENTITY){
		return -1;
	}

	/* Set up kernel stack pointer that corresponds to process about to be executed. */
	kern_stack_ptr = (uint32_t*)(EIGHT_MB - STACK_ROW_SIZE - (EIGHT_KB * next_pid));

	/* Initialize current process Process Control Block at proper location in kernel page. */
	PCB_t * exec_pcb = pcb_init(kern_stack_ptr, next_pid, (PCB_t *)(tss.esp0 & 0xFFFFE000));
	if(NULL == exec_pcb){
		return -1;
	}

	/* Put args into PCB */
	exec_pcb->arg_len = arg_len_count;
	strcpy((int8_t*)exec_pcb->arguments,(const int8_t*)temp_arg_buf);

	/* Set up standard IN/OUT file operations and mark files as in use by modifying 'flags'. */
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

	/* Set up virtual page to hold program information and flush TLB. */
	paging_switch(USER_PROG_VM, STACK_ROW_SIZE * (exec_pcb->process_id + PID_OFF));

	/* Load Program into newly-assigned virtual memory. */
	if(-1 == read_data(exec.inode_number, 0, (uint8_t*)PROG_LOAD_LOC, this_inode->length)){
		return -1;
	}

	pid_bitmap[exec_pcb->process_id] = 1;

	// for(i = 0; i < this_inode->length; i++){
	// 	*((uint8_t*)(PROG_LOAD_LOC + i)) = file_buffer[i];
	// }

	/* Load first instruction location into EIP (reverse order since it's little-endian). */
	eip = ((uint32_t)(file_buffer[EIP_LOC]) << 24) | ((uint32_t)(file_buffer[EIP_LOC - 1]) << 16) | ((uint32_t)(file_buffer[EIP_LOC - 2]) << 8) | ((uint32_t)(file_buffer[EIP_LOC - 3]));

	/* Set TSS values (more for safety than necessity) */
	tss.esp0 = (uint32_t)kern_stack_ptr;
	tss.ss0 = KERNEL_DS;

	/* Save current process ESP/EBP into PCB in order to return to the correct parent process kernel stack. */
  asm volatile("movl %%esp, %0;"
			"movl %%ebp, %1;"
			: "=m"(exec_pcb->kern_esp), "=m"(exec_pcb->kern_ebp)
			:
			: "eax"
			);

	/* Set up stacks before IRET call to run user program. */
	user_prep(eip, USER_STACK_POINTER);

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
int32_t sys_read(int32_t fd, void* buf, int32_t nbytes){
	//first make sure fd in range
	if(fd > MAX_FILES || fd < 0 ) {
		return -1;
	}
	PCB_t* curr_pcb = (PCB_t*)((int32_t)tss.esp0 & 0xFFFFE000);
	//check if the file is actually open
	if(!(curr_pcb->file_array[fd].flags == 1)){
		return -1;
	}

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
int32_t sys_write(int32_t fd, void* buf, int32_t nbytes){
	//first make sure fd in range
	if(fd > MAX_FILES || fd < 0 ) {
		return -1;
	}
	PCB_t* curr_pcb = (PCB_t*)((int32_t)tss.esp0 & 0xFFFFE000);
	//check if the file is actually open
	if(!(curr_pcb->file_array[fd].flags == 1)){
		return -1;
	}

	return curr_pcb->file_array[fd].file_operations.device_write(fd, buf, nbytes);
}

/* sys_open
*		Description: the open system call handler -- opens file
*		Author: Sam
*		Input: filename
*		Ouputs: none
*		Returns: -1 for fail; otherwise return fd
*		Side effect: File is opened and has fd in the fd array
*/
int32_t sys_open(const uint8_t* filename){
	int fd;
	//check for actual input (no null or empty string)
	if(filename == NULL || filename[0] == '\0'){
		return -1;
	}

	/*Create the PCB*/
	PCB_t* curr_pcb = (PCB_t*)((int32_t)tss.esp0 & 0xFFFFE000);

	/*Read file by name*/
	dentry_t this_file;
	if(-1 == read_dentry_by_name(filename, &this_file)){
		return -1;
	}

	/* Return first available file descriptor, if available; else return failure if no file descriptor available. */
	fd = get_first_fd();
	if(fd == -1){
		return -1;
	}

	/* Initialize correct FOP associations */
	switch(this_file.file_type) {
		case REGULAR_FILE_TYPE:
			curr_pcb->file_array[fd].file_operations.device_open = file_open;
			curr_pcb->file_array[fd].file_operations.device_close = file_close;
			curr_pcb->file_array[fd].file_operations.device_read = file_read;
			curr_pcb->file_array[fd].file_operations.device_write = file_write;
			curr_pcb->file_array[fd].file_operations.device_open(filename);
			strcpy((int8_t*)curr_pcb->file_array[fd].fname, (int8_t*)filename);
			curr_pcb->file_array[fd].inode_number = this_file.inode_number;
			curr_pcb->file_array[fd].file_position = 0;
			break;

		case DIRECTORY_FILE_TYPE:
			curr_pcb->file_array[fd].file_operations.device_open = directory_open;
			curr_pcb->file_array[fd].file_operations.device_close = directory_close;
			curr_pcb->file_array[fd].file_operations.device_read = directory_read;
			curr_pcb->file_array[fd].file_operations.device_write = directory_write;
			curr_pcb->file_array[fd].file_operations.device_open(filename);
			strcpy((int8_t*)curr_pcb->file_array[fd].fname, (int8_t*)filename);
			break;

		case RTC_FILE_TYPE:
			curr_pcb->file_array[fd].file_operations.device_open = rtc_open;
			curr_pcb->file_array[fd].file_operations.device_close = rtc_close;
			curr_pcb->file_array[fd].file_operations.device_read = rtc_read;
			curr_pcb->file_array[fd].file_operations.device_write = rtc_write;
			break;

		default:
			return -1;
	}

	return fd;
}

/* sys_close
*		Description: the close system call handler -- closes file
*		Author: Sam
*		Input: fd number
*		Ouputs: none
*		Returns: 0 for pass; -1 for fail
*		Side effect: File is closed and entry in fd freed
*/
int32_t sys_close(int32_t fd){
	/*dont allow users to close stdin/out
	*Call from kern is a flag set to indicate that the call is
	*coming from the kernel (specifically halt here) since halt
	*needs to close stdin/out but we cannot let the users do so
	*/
	if(fd > MAX_FILES || fd < 0 || (!call_from_kern && fd < 2)) {
		call_from_kern = 0;
		return -1;
	}
	call_from_kern = 0;
	PCB_t* curr_pcb = (PCB_t*)((int32_t)tss.esp0 & 0xFFFFE000);
	//check if it is open
	if(!(curr_pcb->file_array[fd].flags == 1)){
		return -1;
	}

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
int32_t sys_getargs(uint8_t* buf, int32_t nbytes){
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
*		Author: Jonathan
*		Description: Maps the text mode video mem into user space at preset virtual addr.
*		Inputs: Pointer to where to put the pointer
*		Return: -1 for fail ... virtual address (const 132MB) upon success
*		Side_effects: Video Mem mapped to 132MB in virtual mem
*/
int32_t sys_vidmap(uint8_t** screen_start){
	//check for non-NULL
	if(screen_start == NULL){
		return -1;
	}

	//Checking valid location not right yet
	if(((uint32_t)screen_start < _128_MB) || ((uint32_t)screen_start >= _132_MB)){
		return -1;
	}


	/*Set screen start pointer to the virtual location of video mem inside the mapped page.
		We are using the 4MB starting at 132 MB because that is,
		at the time this is written, the first 4MB available in memory */
	uint8_t* ptr;
	ptr = (uint8_t*)(_132_MB + VIDEO_LOC);
	*screen_start = ptr;

	/*map based on video number (this depends on whether we have implemented multiple terminals?)
	*Call our paging mapping function ... input 1 is virtual location; 2 is physical location
	*Virtual Location is:  132 MB
	*Physical Location is:   0 MB (Video Mem at 0xB8000)
	*/

	//return the virtual location of page containing video memory
	return _132_MB;

}

/*
*	sys_set_handler
*		Not supported. Returns Fail
*/
int32_t sys_set_handler(int32_t signum, void* handler_address){
	return -1;
}
/*
*	sys_set_handler
*		Not supported. Returns Fail
*/
int32_t sys_sigreturn(void){
	return -1;
}


/* Below are place holders for calls table 
	Return fail to indicate you aren't allowed to do that */
int32_t blank_write(int32_t fd, const void* buf, int32_t nbytes) {
	return -1;
}
int32_t blank_read(int32_t fd, void* buf, int32_t nbytes) {
	return -1;
}
