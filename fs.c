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
#define FS_MOUNTED 0

// Data structures
typedef struct
{
    char name[MAX_FILENAME_LEN]; // File name
    int size;                    // File size
    int start_sector;            // First data block for the file
    int in_use;                  // 1 if file is in use, 0 otherwise
} FileEntry;

typedef struct
{
    int root_dir_block;         // Block number where the root directory starts
    int fat_block;              // Block number where the file allocation table starts
    int free_block_count;       // Number of free blocks
    int magic;                  // Magic number to verify file system integrity
    FileEntry files[MAX_FILES]; // Array of file entries (for managing files)
} superblock_t;

typedef struct
{
    char name[MAX_FILENAME_LEN + 1]; // File name (max 15 characters + null terminator)
    int size;                        // File size
    int first_block;                 // First data block for the file
} directory_entry_t;

typedef struct
{
    int file_descriptor; // Index of the open file in the descriptor table
    int offset;          // Current file pointer
    int file_id;         // The file's index in the directory
} file_descriptor_t;

// Flag for filesystem mount status
extern int fs_mounted;

// Function prototypes
int make_fs(const char *disk_name);   // Create a new file system on disk
int mount_fs(const char *disk_name);  // Mount an existing file system
int umount_fs(const char *disk_name); // Unmount the file system

int fs_create(const char *name);                         // Create a new file
int fs_delete(const char *name);                         // Delete a file
int fs_open(const char *name);                           // Open a file
int fs_close(int fildes);                                // Close a file
int fs_read(int fildes, void *buf, size_t nbyte);        // Read data from a file
int fs_write(int fildes, const void *buf, size_t nbyte); // Write data to a file
int fs_lseek(int fildes, off_t offset);                  // Seek to a specific position in a file
int fs_truncate(int fildes, off_t length);               // Truncate a file to a specific size
int fs_get_filesize(int fildes);                         // Get the size of a file

int fs_list(); // List all files in the file system

// Disk functions (moved to fs.h from disk.c to avoid implicit declaration errors)
int read_block(int block, char *buffer);        // Read a block from disk
int write_block(int block, const char *buffer); // Write a block to disk

#endif // FS_H
