#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define NUM_FILE_OPERATIONS   4
#define FILE_NAME_SIZE        32
#define FILE_TYPE_SIZE        4
#define DENTRY_NUM_SIZE       4
#define INODE_NUMBER_SIZE     4
#define DATA_LENGTH_SIZE      4
#define DENTRY_SIZE           64
#define END_BOOT_BLOCK        63
#define NUM_RESERVED_BYTES    24
#define NUM_RESERVED_BOOT_BYTES 52
#define BLOCK_SIZE            4096

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

struct filesystem_t {
  boot_block_t* boot_block;
  inode_t* inode_start;
  uint8_t* data_block_start;
};

struct boot_block_t {
  uint32_t num_dir_entries;
  uint32_t num_inodes;
  uint32_t num_dblocks;
  uint8_t reserved_bytes[NUM_RESERVED_BOOT_BYTES];
  dentry_t* directory_entries;
};

struct dentry_t {
  uint8_t file_name[FILE_NAME_SIZE];
  uint32_t file_type;
  uint32_t inode_number;
  uint8_t reserved_bytes[NUM_RESERVED_BYTES];
};

 struct inode_t {
  uint32_t length;
  uint8_t* inode_data_blocks;
};

void file_system_init(uint32_t * start_addr);

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
