#ifndef FS_H
#define FS_H

#include <stddef.h>
#include <sys/types.h>

// File system constants
#define MAX_FILES 64
#define MAX_FILENAME_LEN 15
#define BLOCK_SIZE 4096
#define MAX_BLOCKS 8192
#define MAX_FILE_SIZE (4096 * BLOCK_SIZE)

// Data structures
typedef struct
{   
    int root_dir_block; // Block number where the root directory starts
    int fat_block;      // Block number where the file allocation table starts
    int free_block_count;
} superblock_t;

typedef struct
{
    char name[MAX_FILENAME_LEN + 1]; // File name (max 15 characters)
    int size;                        // File size
    int first_block;                 // First data block for the file
} directory_entry_t;

typedef struct
{
    int file_descriptor;
    int offset;  // File pointer
    int file_id; // The file index in the directory
} file_descriptor_t;

// Function prototypes
int make_fs(char *disk_name);
int mount_fs(char *disk_name);
int umount_fs(char *disk_name);

int fs_create(char *name);
int fs_delete(char *name);
int fs_open(char *name);
int fs_close(int fildes);
int fs_read(int fildes, void *buf, size_t nbyte);
int fs_write(int fildes, void *buf, size_t nbyte);
int fs_lseek(int fildes, off_t offset);
int fs_truncate(int fildes, off_t length);
int fs_get_filesize(int fildes);

#endif // FS_H
