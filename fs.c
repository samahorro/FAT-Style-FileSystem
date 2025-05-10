#include "fs.h"
#include "disk.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFAULT_DISK_SECTORS 8192

static superblock_t superblock;
static directory_entry_t root_dir[MAX_FILES];
static file_descriptor_t fd_table[32];
int fs_mounted = 0;

// Function to initialize the file system (create a new file system)
int make_fs(const char *disk_name)
{
    // Create the disk with the specified number of sectors
    if (make_disk(disk_name, DEFAULT_DISK_SECTORS) < 0)
        return -1;

    // Open the newly created disk
    if (open_disk(disk_name, DEFAULT_DISK_SECTORS) < 0)
        return -1;

    // Initialize the superblock
    superblock.root_dir_block = 1;
    superblock.fat_block = 2;
    superblock.free_block_count = MAX_BLOCKS - 2;
    superblock.magic = FS_MAGIC;

    // Write the initialized superblock back to disk
    if (write_block(0, (char *)&superblock) < 0)
    {
        return -1;
    }

    // Initialize the root directory as empty
    memset(root_dir, 0, sizeof(root_dir));
    if (write_block(superblock.root_dir_block, (char *)root_dir) < 0)
    {
        return -1;
    }

    // Close the disk after initialization
    close_disk();
    return 0;
}

// Function to mount an existing file system from a disk
int mount_fs(const char *disk_name)
{
    if (fs_mounted)
    {
        fprintf(stderr, "Filesystem already mounted.\n");
        return -1;
    }

    // Open the specified disk
    if (open_disk(disk_name, DEFAULT_DISK_SECTORS) < 0)
    {
        fprintf(stderr, "Error opening disk.\n");
        return -1;
    }

    char buffer[BLOCK_SIZE];

    // Read the superblock from disk
    if (read_block(0, buffer) < 0)
    {
        fprintf(stderr, "Error reading superblock.\n");
        close_disk();
        return -1;
    }

    // Copy the superblock data into memory
    memcpy(&superblock, buffer, sizeof(superblock_t));

    // Check the filesystem magic number for integrity
    if (superblock.magic != FS_MAGIC)
    {
        fprintf(stderr, "Invalid filesystem magic number.\n");
        close_disk();
        return -1;
    }

    // Load the root directory from the disk
    if (read_block(superblock.root_dir_block, (char *)root_dir) < 0)
    {
        fprintf(stderr, "Error reading root directory.\n");
        close_disk();
        return -1;
    }

    fs_mounted = 1; // Mark the filesystem as mounted
    return 0;
}

// Function to unmount the file system from disk
int umount_fs(const char *disk_name)
{
    if (!fs_mounted)
    {
        fprintf(stderr, "No filesystem is currently mounted.\n");
        return -1;
    }

    char buffer[BLOCK_SIZE];

    // Write the superblock back to disk
    memcpy(buffer, &superblock, sizeof(superblock_t));
    if (write_block(0, buffer) < 0)
    {
        fprintf(stderr, "Error writing superblock.\n");
        return -1;
    }

    // Write the root directory back to disk
    if (write_block(superblock.root_dir_block, (char *)root_dir) < 0)
    {
        fprintf(stderr, "Error writing root directory.\n");
        return -1;
    }

    // Close the disk after unmounting
    if (close_disk() < 0)
    {
        fprintf(stderr, "Error closing disk.\n");
        return -1;
    }

    fs_mounted = 0; // Mark the filesystem as unmounted
    return 0;
}

// Function to create a new file
int fs_create(const char *name)
{
    if (!name || strlen(name) >= MAX_FILENAME_LEN)
        return -1;

    // Check if the file already exists
    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (superblock.files[i].in_use && strcmp(superblock.files[i].name, name) == 0)
            return -1; // File already exists
    }

    // Find an empty slot for the new file
    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (!superblock.files[i].in_use)
        {
            // Initialize the new file entry
            strcpy(superblock.files[i].name, name);
            superblock.files[i].size = 0;
            superblock.files[i].start_sector = -1; // No data allocated yet
            superblock.files[i].in_use = 1;

            // Write the updated superblock back to disk
            return write_block(0, (char *)&superblock);
        }
    }

    return -1; // No space for new files
}

// Function to delete a file from the file system
int fs_delete(const char *name)
{
    // Search for the file in the directory
    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (superblock.files[i].in_use && strcmp(superblock.files[i].name, name) == 0)
        {
            superblock.files[i].in_use = 0;             // Mark file as unused
            return write_block(0, (char *)&superblock); // Write the updated superblock
        }
    }
    return -1; // File not found
}

// Function to list all files in the file system
int fs_list()
{
    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (superblock.files[i].in_use)
        {
            printf("%s\n", superblock.files[i].name); // Print the file name
        }
    }
    return 0;
}

// Function to open a file
int fs_open(const char *name)
{
    // Check if the filesystem is mounted
    if (!fs_mounted)
    {
        fprintf(stderr, "Filesystem not mounted.\n");
        return -1;
    }

    // Search for the file in the filesystem
    int file_id = -1;
    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (superblock.files[i].in_use && strcmp(superblock.files[i].name, name) == 0)
        {
            file_id = i;
            break;
        }
    }

    if (file_id == -1)
    {
        fprintf(stderr, "File not found: %s\n", name);
        return -1;
    }

    // Find an available file descriptor slot
    int fd = -1;
    for (int i = 0; i < 32; ++i)
    {
        if (fd_table[i].file_descriptor == 0)
        { // 0 means unused slot
            fd = i;
            break;
        }
    }

    if (fd == -1)
    {
        fprintf(stderr, "No available file descriptors.\n");
        return -1;
    }

    // Set the file descriptor table entry
    fd_table[fd].file_descriptor = 1; // Mark as open
    fd_table[fd].offset = 0;          // Start from the beginning of the file
    fd_table[fd].file_id = file_id;   // Map to the correct file ID

    return fd; // Return the file descriptor index
}

// Function to close a file
int fs_close(int fildes)
{
    // Check if the file descriptor is valid
    if (fildes < 0 || fildes >= 32 || fd_table[fildes].file_descriptor == 0)
    {
        fprintf(stderr, "Invalid file descriptor.\n");
        return -1;
    }

    // Mark the file descriptor as closed
    fd_table[fildes].file_descriptor = 0;
    fd_table[fildes].offset = 0;   // Reset the offset
    fd_table[fildes].file_id = -1; // Reset the file ID

    return 0; // Successfully closed the file
}

// Function to read data from a file
int fs_read(int fildes, void *buf, size_t nbyte)
{
    if (fildes < 0 || fildes >= MAX_FILES || !superblock.files[fildes].in_use)
        return -1;

    // Implement reading logic, using `read_block()` as necessary to fetch data
    return 0;
}

// Function to write data to a file
int fs_write(int fildes, const void *buf, size_t nbyte)
{
    if (fildes < 0 || fildes >= MAX_FILES || !superblock.files[fildes].in_use)
        return -1;

    // Implement writing logic, using `write_block()` to write data to disk
    return 0;
}

// Function to seek to a specific offset in an open file
int fs_lseek(int fildes, off_t offset)
{
    // Implement seeking logic (adjust file descriptor table entry)
    return 0;
}

// Function to truncate a file to a specific size
int fs_truncate(int fildes, off_t length)
{
    // Implement truncating logic
    return 0;
}

// Function to get the size of an open file
int fs_get_filesize(int fildes)
{
    if (fildes < 0 || fildes >= MAX_FILES || !superblock.files[fildes].in_use)
        return -1;

    return superblock.files[fildes].size;
}
