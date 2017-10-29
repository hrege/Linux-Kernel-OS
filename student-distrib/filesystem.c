#include "filesystem.h"
#include "lib.h"

static uint8_t* file_system_start;
filesystem_t filesystem;
int32_t cur_read_idx = 1;

void file_system_init(uint32_t * start_addr) {
  /* Define start of file system as address calculated in kernel.c */
  filesystem->boot_block_start = start_addr;

  filesystem->boot_block_start->num_dir_entries = *((uint32_t)filesystem->boot_block_start);
  filesystem->boot_block_start->num_inodes = *((uint8_t)filesystem->boot_block_start + DENTRY_NUM_SIZE);
  filesystem->boot_block_start->num_dblocks = *((uint8_t)filesystem->boot_block_start + INODE_NUMBER_SIZE + DENTRY_NUM_SIZE);
  filesystem->boot_block_start->directory_entries = ((uint8_t)filesystem->boot_block_start + DENTRY_SIZE);

  filesystem->inode_start = (uint8_t)start_addr + BLOCK_SIZE;
  filesystem->data_block_start = (uint8_t)start_addr + (BLOCK_SIZE * (filesystem->boot_block_start->num_inodes));
}

int32_t file_open(const uint8_t * filename) {
  int32_t retval;
  dentry_t file_dentry;

  /* Read filename to make sure it exists in directory. */
  retval = read_dentry_by_name(filename, &(file_dentry));
  if(retval == -1) {
    return retval;
  }

  /* Check if parameter represents regular file, not directory or RTC file type. */
  if(file_dentry.file_type != 2){
    printf("Not a regular file\n");
    return -1;
  }

  return 0;
}


int32_t file_close(int32_t fd) {
  return 0;
}


int32_t file_write(int32_t fd, const void* buf, int32_t nbytes) {
  return -1;
}


int32_t file_read(int32_t fd, void* buf, int32_t nbytes, uint8_t * fname) {
  int retval;
  dentry_t file_dentry;
  //Call read_by_name to pass in correct dentry
  retval = read_dentry_by_name(fname, &(file_dentry));  //add in check for retval later
  return read_data(file_dentry.inode_number, 0, buf, nbytes);
}


int32_t directory_open(const uint8_t * filename) {
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


int32_t directory_close(int32_t fd) {
  return 0;
}


int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes) {
  return -1;
}


int32_t directory_read(int32_t fd, void* buf, int32_t nbytes) {
  /* Check that the current entry to read exists. */
  if(cur_read_idx > (filesystem->boot_block_start->num_dir_entries)) {
    return 0;
  }

  dentry_t this_entry;

  /* Copy number of directory entries given by boot block into variable */
  if(read_dentry_by_index(cur_read_idx, &(this_entry)) == 0) {
    strncpy(buf, &(this_entry.file_name), nbytes);   //potential source of error with pointer manipulation
    cur_read_idx++;
    return nbytes;
  }
  return 0;
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
  //if non-existent file or invalid index, return -1
  if(fname == NULL) {
    printf("invalid file\n");
    return -1;
  }

  int i, j; /* Looping variables */

  /* Create temporary entry to hold starting address of directories in boot block. */
  dentry_t temp_dentry;
  temp_dentry = filesystem->boot_block_start->directory_entries;

  /* Loop through each entry in the boot block (size 64 bytes each) and skip the
     statistics portion of the block.
   */
  for(i = 1; i < DENTRY_SIZE; i++) {

      /* Compare file name parameter to current entry in boot block. If names
         don't match, continue to next entry in boot block.
       */
      if(strncmp(fname, &(temp_dentry.file_name), FILE_NAME_SIZE) == 0) {//Make sure temp_addr and fname fit into the int8_t inputs
          break;
      }

      /* Once at end of boot block, file does not exist in directory. */
      if(i == END_BOOT_BLOCK){
        printf("file not found\n");
        return -1;
      }

      /* Increment temp_addr to point to the next dentry to check */
      temp_dentry += DENTRY_SIZE;
    }
  /* Copy matching file name into the temporary dentry. */
  strncpy(dentry->file_name, fname, FILE_NAME_SIZE);

  /* Set file type and inode number of dentry according to current entry in directory. */
  dentry->file_type = *((uint8_t)temp_dentry + FILE_NAME_SIZE);
  dentry->inode_number = *((uint8_t)temp_dentry + FILE_NAME_SIZE + FILE_TYPE_SIZE);

  return 0;
}

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
  /* If non-existent file or invalid index, return error */
  if((index > NUM_FILES || index < 0)) {
    printf("invalid file\n");
    return -1;
  }

  /* Create temporary pointer to hold starting address of current directory entry based on index. */
  dentry_t temp_addr = filesystem->boot_block_start->directory_entries[index];

  /* Set the file name of the passed in dentry to the fname parameter. */
  strncpy(dentry->file_name, &(temp_addr.file_name), FILE_NAME_SIZE);

  /* Set file type and inode number of dentry according to current entry in directory. */
  dentry->file_type = *((uint8_t)temp_addr + FILE_NAME_SIZE);
  dentry->inode_number = *((uint8_t)temp_addr + FILE_NAME_SIZE + FILE_TYPE_SIZE);

  return 0;
}

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
  /* Initialize loop variables */
  int i, j;

  /* Check if inode parameter is within valid range of inodes */
  int num_inodes = *((uint8_t)file_system_start + DENTRY_NUM_SIZE);
  if(inode < 0 || inode > (filesystem->boot_block_start->num_inodes - 1)) {
    printf("Inode out of bounds!\n");
    return -1;
  }
  if(offset < 0 || offset > length) {
    return -1;
  }

  /* Store inode information using the "inode" index */
  inode_t* this_inode;
  this_inode = (uint8_t)filesystem->inode_start + (inode * BLOCK_SIZE);
  this_inode.length = *((uint32_t)this_inode);
  this_inode->inode_data_blocks = (uint8_t)this_inode + DATA_LENGTH_SIZE;

  /* Determine the total number of data blocks based on length of file */
  uint32_t num_inode_dblocks;                                                 //TODO Double-check
  num_inode_dblocks = this_inode.length/BLOCK_SIZE;
  if((this_inode.length % BLOCK_SIZE) > 0){
    num_inode_dblocks++;
  }

  /* Check if each data block represented by inode is within bounds. */
  uint32_t curr_block;
  for(i = 0; i < num_inode_dblocks; i++) {
    curr_block = *((uint8_t)(this_inode->inode_data_blocks) + i*DATA_LENGTH_SIZE);
    if(curr_block < 0 || curr_block >= filesystem->boot_block_start.num_dblocks){
      printf("Data block not within bounds\n");
      return -1;
    }
  }

  /* Set buff_ptr to start of buffer to begin */
  void* buff_ptr = buf;
  data_block_t* block_data;

  /*Loop through data blocks in sequence in inode */
  for(i = 0; i < num_inode_dblocks; i++) {
    curr_block = *((uint8_t)this_inode->inode_data_blocks + i*DATA_LENGTH_SIZE); //which number data we're looking at
    block_data = ((uint8_t)filesystem->data_block_start) + (curr_block * BLOCK_SIZE);
    buff_ptr = (uint8_t)buf + i*BLOCK_SIZE;
    *buff_ptr = *block_data;
  }

  return length;
}
