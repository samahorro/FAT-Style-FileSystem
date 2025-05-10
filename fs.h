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
#define FS_MAGIC 0x12345678
#define SECTOR_SIZE 512
#define FS_MOUNTED = 0

// Data structures
typedef struct {
    char name[MAX_FILENAME_LEN];
    int size;
    int start_sector;
    int in_use;
} FileEntry;

typedef struct {   
    int root_dir_block; // Block number where the root directory starts
    int fat_block;      // Block number where the file allocation table starts
    int free_block_count;
    int magic;
    FileEntry files[MAX_FILES];
} superblock_t;

typedef struct {
    char name[MAX_FILENAME_LEN + 1]; // File name (max 15 characters)
    int size;                        // File size
    int first_block;                 // First data block for the file
} directory_entry_t;

typedef struct {
    int file_descriptor;
    int offset;  // File pointer
    int file_id; // The file index in the directory
} file_descriptor_t;

// Flag for filesystem mount status
extern int fs_mounted;

// Function prototypes
int make_fs(char *disk_name);
int mount_fs(char *disk_name);
int umount_fs(char *disk_name);

int fs_create(const char *name);
int fs_delete(const char *name);
int fs_open(char *name);
int fs_close(int fildes);
int fs_read(int fildes, void *buf, size_t nbyte);
int fs_write(int fildes, void *buf, size_t nbyte);
int fs_lseek(int fildes, off_t offset);
int fs_truncate(int fildes, off_t length);
int fs_get_filesize(int fildes);

#endif // FS_H
