#include "filesystem.h"
#include "lib.h"

static uint8_t* file_system_start;
static dentry_t* dir_dentry;
static dentry_t* file_dentry;
int32_t cur_read_idx = 1;

void file_system_init(uint32_t * start_addr) {
  /* Define start of file system as address calculated in kernel.c */
  filesystem_t filesystem;
  filesystem->boot_block;
  file_system_start = start_addr;
}

int32_t file_open(const uint8_t * filename) {
  int32_t retval;

  /* Read filename to make sure it exists in directory. */
  retval = read_dentry_by_name(filename, file_dentry);
  if(retval == -1) {
    return retval;
  }

  /* Check if parameter represents regular file, not directory or RTC file type. */
  if(file_dentry->file_type != 2){
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


int32_t file_read(int32_t fd, void* buf, int32_t nbytes) {
  // int total_bytes_read = 0;   //Count of bytes read
  // int bytes_read = 1;         //Number of bytes read per read_data call

  /* Continue reading data until number of bytes read is size of nbytes. */
  // while(bytes_read != 0) {
  //   bytes_read = read_data(file_dentry->inode, total_bytes_read, buf, nbytes - total_bytes_read);
  //   total_bytes_read += bytes_read;
  // }
  // return total_bytes_read;

  return read_data(file_dentry->inode, 0, buf, nbytes);
}


int32_t directory_open(const uint8_t * filename) {
  int32_t retval;

  /* Read filename to make sure it exists in directory. */
  retval = read_dentry_by_name(filename, dir_dentry);
  if(retval == -1) {
    return retval;
  }

  /* Check if parameter represents directory file, not regular or RTC file type. */
  if(dir_dentry->file_type != 1){
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
  //read files using filename by filename, including "." (first entry)
  //uses read_dentry_by_index
  int32_t num_dir_entries;

  /* Copy number of directory entries given by boot block into variable */
  //strncpy(num_dir_entries, file_system_start, DENTRY_NUM_SIZE);

  num_dir_entries = *(file_system_start);

  /* Check that the current entry to read exists. */
  if(cur_read_idx > num_dir_entries){
    return 0;
  }

  dentry_t* this_entry;
  if(read_dentry_by_index(cur_read_idx, this_entry) == 0) {
    strncpy(buf, &(this_entry->file_name), nbytes);   //potential source of error with pointer manipulation

    cur_read_idx++;
    return nbytes;
  }
  return 0;
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
  //if non-existent file or invalid index, return -1
  if(fname == NULL || dentry == NULL) {
    printf("invalid file\n");
    return -1;
  }

  /* Looping variables */
  int i, j;

  /* Create temporary pointer to hold starting address of file system image. */
  dentry_t * temp_addr = file_system_start;

  /* Loop through each entry in the boot block (size 64 bytes each) and skip the
     statistics portion of the block.
   */
  for(i = 1; i < DENTRY_SIZE; i++) {
      temp_addr += DENTRY_SIZE;

      /* Compare file name parameter to current entry in boot block. If names
         don't match, continue to next entry in boot block.
       */
      //for(j = 0; j < FILE_NAME_SIZE; j++) {
      //  if(*fname[j] != temp_addr[j]) {
      //    name_flag = 1;
      //    break;
      //  }
      //}
      if(strncmp(fname, &(temp_addr->file_name), FILE_NAME_SIZE) == 0) {//Make sure temp_addr and fname fit into the int8_t inputs
          break;
      }

      /* Once at end of boot block, file does not exist in directory. */
      if(i == END_BOOT_BLOCK){
        printf("file not found\n");
        return -1;
      }
    }
  /* Copy matching file name into the temporary dentry. */
  //for(i = 0; i < FILE_NAME_SIZE; i++){
  //  dentry->file_name[i] = (*fname)[i];
  //}
  //dentry->file_name = *fname;
  strncpy(dentry->file_name, fname, 32);

  /* Set file type and inode number of dentry according to current entry in directory. */
  temp_addr += FILE_NAME_SIZE; //double check to see if this increments by bytes v. bits
  dentry->file_type = *temp_addr;
  temp_addr += FILE_TYPE_SIZE;
  dentry->inode_number = *temp_addr;

  return 0;
}

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
  /* If non-existent file or invalid index, return error */
  if((index > 62 || index < 0) || dentry == NULL) {
    printf("invalid file\n");
    return -1;
  }

  /* Create temporary pointer to hold starting address of file system image. */
  dentry_t * temp_addr = dir_dentry;
  temp_addr += (index+1)*DENTRY_SIZE;

  /* Set the file name of the passed in dentry to the fname parameter. */
  //for(i = 0; i < FILE_NAME_SIZE; i++){
  //dentry->file_name[i] = (*temp_addr)[i];   //proper syntax??????????????????????????????????
  //}
  strncpy(dentry->file_name, temp_addr, FILE_NAME_SIZE);

  /* Set file type and inode number of dentry according to current entry in directory. */
  temp_addr += FILE_NAME_SIZE; //double check to see if this increments by bytes v. bits
  dentry->file_type = *temp_addr;
  temp_addr += FILE_TYPE_SIZE;
  dentry->inode_number = *temp_addr;

  return 0;
}

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
  /* Check if inode parameter is within valid range of inodes */
  int num_inodes = *(file_system_start + DENTRY_NUM_SIZE);
  if(inode < 0 || inode > (num_inodes - 1)) {
    printf("Inode out of bounds!\n");
    return -1;
  }
  if(offset < 0 || offset > length) {
    return -1;
  }

  /* Store starting address of inode in reference to the start of the boot block. */
  int32_t* inode_addr;
  inode_addr = file_system_start + (inode+1)*BLOCK_SIZE;

  /* Calculate total number of data blocks and length of data blocks in bytes. */
  int num_dblocks = *(file_system_start + DENTRY_NUM_SIZE + INODE_NUMBER_SIZE);
  int32_t dblock_length = *(inode_addr);

  int i;
  int32_t curr_block;

  /* Check if each data block represented by inode is within bounds. */
  for(i = 0; i < (num_dblocks/4); i++) {
    curr_block = *(inode_addr + INODE_NUMBER_SIZE*i);
    if(curr_block < 0 || curr_block >= num_dblocks){
      printf("Data block not within bounds\n");
      return -1;
    }
  }
  int32_t data_block_addr;
  data_block_addr = inode_addr + (num_inodes*BLOCK_SIZE);
  int num_bytes_read = 0;
  /*
    Reading up to 'length' bytes starting from position 'offset' in the file with
    inode number 'inode' and returning the number of bytes read and placed in 'buf'.
  */
  for(i = 0; i < (num_dblocks/4); i++) {
    curr_block = data_block_addr + (inode_addr + (INODE_NUMBER_SIZE*i)); //set curr address to first data block in inode
    buf[i] = *(curr_block);
    num_bytes_read += BLOCK_SIZE;  //what if length doesn't fit a whole block??
    if(num_bytes_read == length) {
      break;
    }
  }
  return num_bytes_read;
}
