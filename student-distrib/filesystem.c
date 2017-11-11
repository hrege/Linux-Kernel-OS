#include "lib.h"
#include "filesystem.h"

filesystem_t filesystem;  /* File system object that contains pointers to each portion of system */
int32_t cur_read_idx = 0; /* Read index to determine if directory has been completely read */
uint32_t number_of_files; /* Global variable to hold the current number of files in directory */

/* void file_system_init(uint32_t * start_addr)
  Description: Initializes the file system to point to the different
               areas (boot block, data blocks, etc.).
  Author: Hershel/Austin
  Inputs: starting address for the file system
  Outputs: none
  Side Effects: sets all filesystem pointers to correct locations based on filesys.img
  Return Value: none
 */
void file_system_init(uint32_t* start_addr) {
  /* Define start of file system as address calculated in kernel.c */
  if(start_addr == NULL) {
    printf("Null start_addr\n");
    return;
  }
  filesystem.boot_block_start = (boot_block_t *)start_addr;
  number_of_files = (uint32_t)(filesystem.boot_block_start->num_dir_entries);

  /* Set pointers to the beginning of the inodes and the data blocks in memory. */
  filesystem.inode_start = (inode_t *)((uint8_t *)start_addr + BLOCK_SIZE);
  filesystem.data_block_start = ((uint8_t *)start_addr + ((uint8_t)filesystem.boot_block_start->num_inodes) * BLOCK_SIZE);
}


/*
 *  Description: Creates PCB data structure for current process being handled and
 *               maps to proper location in kernel page.
 *  Author: Hershel
 *  Inputs: start_addr - pointer to end of kernel page
 *          p_id - ID for current process being handled
 *          parent_PCB - pointer to kernel stack of parent process
 *  Outputs: none
 *  Side Effects: Creates new PCB for current process in address specified by
 *                virtual memory paging.
 *  Return Value: Returns 0 on success, -1 on failure.
 */

int pcb_init(uint32_t* start_addr, uint32_t p_id, uint32_t* parent_PCB) {
  /* Return invalid process ID if it doesn't fall in given range. */
  if(p_id != 0 && p_id != 1) {
    return -1;
  }

  /* Create new PCB struct and set process id. */
  PCB_t new_pcb;
  new_pcb.process_id = p_id;

  /* Decide what to set as Parent PCB pointer - if running SHELL, then set to NULL
     otherwise, point to proper offset in kernel stack. */
  if(p_id == 0) {
    new_pcb.parent_process = NULL;
    *((PCB_t*)(((uint8_t*)start_addr) + (FOUR_MB - EIGHT_KB))) = new_pcb;
  }
  else {
    /* FIX - need to store extra reference to child process (stack pointer of parent)*/
    new_pcb.parent_process = parent_PCB; //Offset starting address by 4MB - 8kB for start of parent PCB
    *(PCB_t*)((uint8_t*)start_addr + (FOUR_MB - (p_id*EIGHT_KB))) = new_pcb;
  }
  return 0;
}

/* int32_t file_open(const uint8_t * filename)
  Description: Opens files from directory and checks the file type to affirm
               only user files are opened.
  Author: Hershel/Austin
  Inputs: filename - File name of the file to be opened
  Outputs: Prints if invalid file
  Side Effects: none
  Return Value: returns 0 if successful open, -1 if error
 */
int32_t file_open(const uint8_t* filename) {
  if(filename == NULL) {
    printf("Null filename\n");
    return -1;
  }
  int32_t retval;
  dentry_t file_dentry;

  /* Read filename to make sure it exists in directory. */
  retval = read_dentry_by_name(filename, &(file_dentry));
  if(retval == -1) {
    return retval;
  }

  /* Check if parameter represents regular file, not directory or RTC file type. */
  if(file_dentry.file_type != REGULAR_FILE_TYPE){
    printf("Not a regular file\n");
    return -1;
  }

  return 0;
}

/* int32_t file_close(int32_t fd)
  Description: Close files based on file descriptor parameter passed in
  Author: Hershel/Austin
  Inputs: fd - file descriptor defining which file needs to be opened
  Outputs: none
  Side Effects: none
  Return Value: Returns 0 on success.
 */
int32_t file_close(int32_t fd) {
  return 0;
}

/* int32_t file_write(int32_t fd, const void* buf, int32_t nbytes)
  Description: Since system is read-only, write always returns failure
  Author: Hershel/Austin
  Inputs: fd - file descriptor
          buf - buffer to hold the data to be written
          nbytes - number of bytes to write
  Outputs: none
  Side Effects: none
  Return Value: Returns -1 because system is read-only
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes) {
  if(buf == NULL) {
    printf("Null buffer pointer\n");
    return -1;
  }
  return -1;
}

/* int32_t file_read(int32_t fd, void* buf, int32_t nbytes, uint8_t* fname)
  Description: Reads contents of file from directory
  Author: Hershel/Austin
  Inputs: fd - file descriptor defining which file needs to be read
          buf - buffer to hold the values being read
          nbytes - number of bytes to be read
          fname - file name to read from
  Outputs: none
  Side Effects: Checks if file exists in current directory
  Return Value: Returns call to read_data, which returns number of bytes read successfully.
 */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes, uint8_t* fname) {
  if(buf == NULL) {
    printf("Null buffer pointer\n");
    return -1;
  }
  if(fname == NULL) {
    printf("Null fname\n");
    return -1;
  }
  int retval;
  dentry_t file_dentry;

  /* Call read_by_name to pass in correct dentry */
  retval = read_dentry_by_name(fname, &(file_dentry));
  if(retval == -1) {
    return retval;
  }
  int bytes_read = 0;
  /* Call read data to read from file inode*/
  while(bytes_read < nbytes) {
   bytes_read += read_data(file_dentry.inode_number, bytes_read, buf, nbytes);
  }
  return bytes_read;
}

/* file_check(uint8_t* fname)
  Description: Verifies that the file given by fname is an
               executable file (by checking the first 4 bytes
               to verify that file contains "ELF")
  Arthur: Austin
  Inputs: fname - file name to be checked
  Outputs: none
  Side Effects: None
  Return Value: Returns 0 if successful, -1 if failed
 */

int32_t file_load(uint8_t * fname, uint8_t* addr){
	/* Initialize local variables */
  dentry_t file_dentry;
  inode_t* this_inode;

	/* Check for invalid inputs */
	if( fname == NULL ){
    printf("invalid file\n");
		return -1;
  }

  /* Extract dentry information using the filename passed in. */
	if(read_dentry_by_name(fname, &(file_dentry)) == -1){
		return -1;
	}

  /* Load inode data (length) */
  this_inode = (inode_t*)((uint8_t*)filesystem.inode_start + (file_dentry.inode_number * BLOCK_SIZE));
  this_inode->length = *((uint32_t*)this_inode);
  int bytes_read = 0;

  /* Load the entire file at the address passed in. */
  while(bytes_read < this_inode->length) {
    bytes_read += read_data(file_dentry.inode_number, bytes_read, addr, this_inode->length);
  }

}

// Not necessary for execute, but kept as insurance
// /* file_load(uint8_t * fname, uint32_t* addr)
//   Description: Loads program image from blocks into contiguous memory
//   Author: Austin
//   Inputs: fname - file name to be loaded
//           addr - address to write data to
//   Outputs: none
//   Side Effects: Loads file into memory at addr location
//   Return Value: 0 if successful, -1 if failed
//  */
// int32_t file_load(uint8_t* fname, void* addr){
//   /* Check for invalid inputs */
// 	if( fname == NULL ){
//     printf("invalid file\n");
// 		return -1;
//   }

//   if(addr == NULL) {
//     printf("Null address\n");
//     return -1;
//   }

//   /* Initialize local variables */
//   dentry_t file_dentry;
//   inode_t* this_inode;
//   uint32_t bytes_read = 0;

// 	/* Extract dentry information using the filename passed in. */
// 	if(read_dentry_by_name(fname, &(file_dentry)) == -1){
// 		return -1;
// 	}

//   /* Load inode data (length) */
//   this_inode = (inode_t*)((uint8_t*)filesystem.inode_start + (file_dentry.inode_number * BLOCK_SIZE));
//   this_inode->length = *((uint32_t*)this_inode);

//   /* Load the entire file at the address passed in. */
//   while(bytes_read < this_inode->length) {
//     bytes_read += read_data(file_dentry.inode_number, bytes_read, addr, this_inode->length);
//    }

// 	return 0;
// }

/* int32_t directory_open(const uint8_t * filename)
  Description: Opens directory structure based on directory name
  Author: Hershel/Austin
  Inputs: filename - name of directory to be opened
  Outputs: Prints on failure to open directory
  Side Effects: Checks to see if filename exists in directory
  Return Value: returns 0 on success and -1 on failure
 */
int32_t directory_open(const uint8_t* filename) {
  if(filename == NULL) {
    printf("Null filename\n");
    return -1;
  }
  int32_t retval;
  dentry_t dir_dentry;
  /* Read filename to make sure it exists in directory. */
  retval = read_dentry_by_name(filename, &(dir_dentry));
  if(retval == -1) {
    return retval;
  }

  /* Check if parameter represents directory file, not regular or RTC file type. */
  if(dir_dentry.file_type != 1){
    printf("Not a directory file\n");
    return -1;
  }
  return 0;
}

/* int32_t directory_close(int32_t fd)
  Description: Closes directory, reversing effects of directory_open
  Author: Hershel/Austin
  Inputs: fd - file descriptor determines which file needs to be closed
  Outputs: none
  Side Effects: none
  Return Value: Returns 0 since always successful
 */
int32_t directory_close(int32_t fd) {
  return 0;
}

/* int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes)
  Description: Writes to directory, but always fails since it's a read-only
               file structure.
  Author: Hershel/Austin
  Inputs: fd - file descriptor determines which file needs to be closed
          buf - buffer to hold the values being read
          nbytes - number of bytes to be read
  Outputs: none
  Side Effects: none
  Return Value: Returns -1 because call always fails
 */
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes) {
  if(buf == NULL) {
    printf("Null buffer pointer\n");
    return -1;
  }
  return -1;
}

/* int32_t directory_read(int32_t fd, void* buf, int32_t nbytes)
  Description: Reads all files from current directory, using call to
               read_dentry_by_index to make sure file exists in directory.
  Author: Hershel/Austin
  Inputs: fd - file descriptor determines which file needs to be closed
          buf - buffer to hold the values being read
          nbytes - number of bytes to be read
  Outputs: none
  Side Effects: Copies current file name into buffer
  Return Value: Return nbytes if file exists, otherwise returns 0 if all
                existing files have been read.
 */
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes) {
  if(buf == NULL) {
    printf("Null buffer pointer\n");
    return -1;
  }
  /* Check that the current entry to read exists. */
  if(cur_read_idx > (filesystem.boot_block_start->num_dir_entries)) {
    return 0;
  }

  dentry_t this_entry;

  /* Check if current directory index exists, then copy file name into buffer. */
  if(read_dentry_by_index(cur_read_idx, &(this_entry)) == 0) {
    strncpy((int8_t *)buf, (int8_t *)&(this_entry.file_name), nbytes);
    cur_read_idx++;
    return nbytes;
  }
  return 0;
}

/* int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
  Description: Check if file exists in directory by comparing parameter file name
               to each entry in directory.
  Author: Hershel/Austin
  Inputs: fname - pointer to file name to be checked
          dentry - pointer to dentry struct to hold information of file being checked
  Outputs: none
  Side Effects: Updates dentry parameter with all file metadata.
  Return Value: Returns 0 on success, and returns -1 on failure.
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
  /* If non-existent file, return -1 */
  if(fname == NULL) {
    printf("invalid file\n");
    return -1;
  }
  if(dentry == NULL) {
    printf("Null dentry\n");
    return -1;
  }

  int i; /* Looping variables */

  /* Create temporary entry to hold starting address of directories in boot block. */
  dentry_t* temp_dentry;
  temp_dentry = filesystem.boot_block_start->directory_entries;

  /*
     Loop through each entry in the boot block (size 64 bytes each) and skip the
     statistics portion of the block.
   */
  for(i = 1; i < DENTRY_SIZE; i++) {

      /*
         Compare file name parameter to current entry in boot block. If names
         don't match, continue to next entry in boot block.
       */
      if(strncmp((int8_t *)fname, (int8_t *)temp_dentry->file_name, FILE_NAME_SIZE) == 0) {
          break;
      }

      /* Once at end of boot block, file does not exist in directory. */
      if(i == END_BOOT_BLOCK){
        printf("file not found\n");
        return -1;
      }

      /* Increment temp_dentry to point to the next dentry to check. */
      temp_dentry++;
    }
  /* Copy matching file name into the temporary dentry. */
  strncpy((int8_t *)dentry->file_name, (int8_t *)fname, strlen((int8_t *)fname));

  /* Set file type and inode number of dentry according to current entry in directory. */
  dentry->file_type = *((uint8_t *)temp_dentry + FILE_NAME_SIZE);
  dentry->inode_number = *((uint8_t *)temp_dentry + FILE_NAME_SIZE + FILE_TYPE_SIZE);

  return 0;
}

/* int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry)
  Description: Checks if index parameter represents a file in the current directory.
  Author: Hershel/Austin
  Inputs: index - file index to be checked
          dentry - pointer to dentry struct to hold the metadata of file if found
  Outputs: none
  Side Effects: Updates dentry struct with all file metadata if file found
  Return Value: Return 0 on success, returns -1 on failure.
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
  /* If non-existent file or invalid index, return error */
  if((index > number_of_files || index < 0)) {
    printf("invalid file\n");
    return -1;
  }
  if(dentry == NULL) {
    printf("Null dentry\n");
    return -1;
  }

  /* Create temporary pointer to hold starting address of current directory entry based on index. */
  dentry_t* temp_addr = &(filesystem.boot_block_start->directory_entries[index]);

  /* Set the file name of the passed in dentry to the fname parameter. */
  strncpy((int8_t*)dentry->file_name, (int8_t*)(temp_addr->file_name), FILE_NAME_SIZE);

  /* Set file type and inode number of dentry according to current entry in directory. */
  dentry->file_type = *((uint8_t*)temp_addr + FILE_NAME_SIZE);
  dentry->inode_number = *((uint8_t*)temp_addr + FILE_NAME_SIZE + FILE_TYPE_SIZE);

  return 0;
}

/* int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
  Description: Reads data from filesystem, checking each file's inodes and
               corresponding data blocks to find relevant data.
  Author: Hershel/Austin
  Inputs: inode - file inode number that contains information about file data
          offset - beginning point at which file should be read
          buf - buffer to hold the read data values
          length - the number of bytes to be read from file
  Outputs: none
  Side Effects: Prints error if inode is invalid
  Return Value: Returns number of bytes read on success, returns -1 on failure.
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
  if(buf == NULL) {
    printf("Null buffer pointer\n");
    return -1;
  }
  /* Initialize loop variables */
  int i;
  int j;
  uint32_t size_left;

  size_left = length - offset;

  /* Check if inode parameter is within valid range of inodes */
  if(inode < 0 || inode > (filesystem.boot_block_start->num_inodes - 1)) {
    printf("Inode out of bounds!\n");
    return -1;
  }
  if(offset < 0 || offset > length) {
    return -1;
  }

  /* Store inode information using the "inode" index */
  inode_t* this_inode;
  this_inode = (inode_t*)((uint8_t*)filesystem.inode_start + (inode * BLOCK_SIZE));
  this_inode->length = *((uint32_t*)this_inode);

  /* Determine the total number of data blocks based on length of file */
  uint32_t num_inode_dblocks;
  num_inode_dblocks = this_inode->length/BLOCK_SIZE;
  if((this_inode->length % BLOCK_SIZE) > 0){
    num_inode_dblocks++;
  }

  /* Check if each data block represented by inode is within bounds. */
  uint32_t curr_block;
  for(i = 0; i < num_inode_dblocks; i++) {
    curr_block = *((uint32_t*)((uint8_t*)this_inode + (i + 1)*DATA_LENGTH_SIZE));
    if(curr_block < 0 || curr_block >= filesystem.boot_block_start->num_dblocks){
      printf("Data block not within bounds\n");
      return -1;
    }
  }

  /* Set buff_ptr to start of buffer to begin */
  data_block_t* block_data;

  /*Loop through data blocks in sequence in inode */
  for(i = 0; i < num_inode_dblocks; i++) {
    curr_block =  *((uint32_t*)((uint8_t*)this_inode + (i + 1)*DATA_LENGTH_SIZE)); //which number data we're looking at
    block_data = (data_block_t *)(filesystem.data_block_start + ((curr_block + 1) * BLOCK_SIZE));
    for(j = 0; j < BLOCK_SIZE; j++) {
      *(buf + i*BLOCK_SIZE + j + offset) = block_data->data[j];
      size_left--;
      if(size_left == 0){
        break;
      }
    }
    if(size_left == 0){
      break;
    }
  }

  return length - offset;
}
