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
#define NUM_FILES             62
#define NUM_RESERVED_BYTES    24
#define NUM_RESERVED_BOOT_BYTES 52
#define BLOCK_SIZE            4096
#define NUM_DIRECTORY_ENTRIES 63

extern uint32_t number_of_files;

typedef struct dentry_t {
  uint8_t file_name[FILE_NAME_SIZE];
  uint32_t file_type;
  uint32_t inode_number;
  uint8_t reserved_bytes[NUM_RESERVED_BYTES];
} dentry_t;

typedef struct inode_t {
  uint32_t length;
  uint32_t* inode_data_blocks;
} inode_t;

typedef struct boot_block_t {
  uint32_t num_dir_entries;
  uint32_t num_inodes;
  uint32_t num_dblocks;
  uint8_t reserved_bytes[NUM_RESERVED_BOOT_BYTES];
  dentry_t directory_entries[NUM_DIRECTORY_ENTRIES];
} boot_block_t;

typedef struct filesystem_t {
  boot_block_t* boot_block_start;
  inode_t* inode_start;
  uint8_t* data_block_start;
} filesystem_t;

extern filesystem_t filesystem;

typedef struct data_block_t {
  uint8_t data[BLOCK_SIZE];
} data_block_t;

void file_system_init(uint32_t * start_addr);

int32_t file_open(const uint8_t * filename);
int32_t file_close(int32_t fd);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t file_read(int32_t fd, void* buf, int32_t nbytes, uint8_t * fname);

int32_t directory_open(const uint8_t * filename);
int32_t directory_close(int32_t fd);
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes);

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif
