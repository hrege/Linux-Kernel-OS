#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define NUM_FILE_OPERATIONS   4
#define FILE_NAME_SIZE        32
#define FILE_TYPE_SIZE        4
#define DENTRY_SIZE           64
#define END_BOOT_BLOCK        63

// struct task_t {
//   file_operations_t * file_operations;
//   uint32_t inode_number;
//   uint32_t file_position;
//   int flags;
// };
//
// struct file_operations_t {
//   int (*file_open)();
//   int (*file_close)();
//   int (*file_read)();
//   int (*file_write)();
//   int (*jump_table[NUM_FILE_OPERATIONS])() = {file_open, file_close, file_read, file_write};
// };

struct dentry_t {
  uint8_t file_name[FILE_NAME_SIZE];
  uint32_t file_type;
  uint32_t inode_number;
  //do we need to allocate the last 24 bytes?
};

struct inode_t {
  uint32_t length;
  
};

int32_t file_open(const uint8_t * filename);
int32_t file_close(int32_t fd);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t file_read(int32_t fd, void* buf, int32_t nbytes);

int32_t directory_open(const uint8_t * filename);
int32_t directory_close(int32_t fd);
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes);

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif
