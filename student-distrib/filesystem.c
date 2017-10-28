#include "filesystem.h"

static dentry_t* dir_dentry = 0x1BADB002;

int32_t file_open(const uint8_t * filename) {
  //initialize any temporary structures
  //use read_dentry_by_name
  return 0;
}


int32_t file_close(int32_t fd) {
  //undo open function
  return 0;
}


int32_t file_write(int32_t fd, const void* buf, int32_t nbytes) {
  return -1;  //do nothing
}


int32_t file_read(int32_t fd, void* buf, int32_t nbytes) {
  //read count bytes of data from file into buf
  //using read_data
  return 0;
}


int32_t directory_open(const uint8_t * filename) {
  //opens directory file (note file types)
  //uses read_dentry_by_name
  int32_t retval;
  retval = read_dentry_by_name(filename, dir_dentry);
  if(retval == -1) {
    return retval;
  }
  if(dir_dentry->file_type != 1){
    printf("Not a directory file\n");
    return -1;
  }
  return 0;
}


int32_t directory_close(int32_t fd) {
  //probably does nothing?
  return 0;
}


int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes) {
  return -1;
}


int32_t directory_read(int32_t fd, void* buf, int32_t nbytes) {
  //read files using filename by filename, including "." (first entry)
  //uses read_dentry_by_index
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

  /* Flag to determine if file name was found in directory. */
  int name_flag = 0;

  /* Create temporary pointer to hold starting address of file system image.
     Set the file name of the passed in dentry to the fname parameter.
   */
  dentry_t * temp_addr = dir_dentry;
  dentry->file_name = *fname;

  /* Loop through each entry in the boot block (size 64 bytes each) and skip the
     statistics portion of the block.
   */
  for(i = 1; i < DENTRY_SIZE; i++) {
      temp_addr += DENTRY_SIZE*i;

      /* Compare file name parameter to current entry in boot block. If names
         don't match, continue to next entry in boot block.
       */
      for(j = 0; j < FILE_NAME_SIZE; j++) {
        if(*fname[j] != temp_addr[j]) {
          name_flag = 1;
          break;
        }
      }

      /* If flag is not set, then we've found a matching file name! */
      if(name_flag == 0) {
        break;
      }

      /* Once at end of boot block, file does not exist in directory. */
      if(i == END_BOOT_BLOCK){
        printf("file not found\n");
        return -1;
      }
      /* Reset flag to continue looking for matching file name in next entry. */
      name_flag = 0;
  }

  /* Set file type and inode number of dentry according to current entry in directory. */
  temp_addr += FILE_NAME_SIZE; //double check to see if this increments by bytes v. bits
  dentry->file_type = *temp_addr;
  temp_addr += FILE_TYPE_SIZE;
  dentry->inode_number = *temp_addr;

  return 0;
}

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
  //if non-existent file or invalid index, return -1
  if((index > 62 || index < 0) || dentry == NULL) {
    printf("invalid file\n");
    return -1;
  }

  /* Create temporary pointer to hold starting address of file system image.
     */
  dentry_t * temp_addr = dir_dentry;
  temp_addr += (index+1)*DENTRY_SIZE;

  /* Set the file name of the passed in dentry to the fname parameter.
    */
  for(i = 0; i < FILE_NAME_SIZE; i++){
    dentry->file_name[i] = (*temp_addr)[i];   //proper syntax??????????????????????????????????
  }

  /* Set file type and inode number of dentry according to current entry in directory. */
  temp_addr += FILE_NAME_SIZE; //double check to see if this increments by bytes v. bits
  dentry->file_type = *temp_addr;
  temp_addr += FILE_TYPE_SIZE;
  dentry->inode_number = *temp_addr;

  return 0;
}

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
  //if inode not within valid range, return -1
  //Need to also check if inode


}
