// fs.c
#include "fs.h"
#include "disk.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static superblock_t superblock;
static directory_entry_t root_dir[MAX_FILES];
static file_descriptor_t fd_table[32];
int fs_mounted = 0;

int make_fs(char *disk_name) {
    if (make_disk(disk_name) < 0) { return -1; }
    if (open_disk(disk_name) < 0) { return -1; }

    // Initialize superblock
    superblock.root_dir_block = 1;
    superblock.fat_block = 2;
    superblock.free_block_count = MAX_BLOCKS - 2;
    superblock.magic = FS_MAGIC;
    
    // Write initialized superblock back to disk
    if (write_block(0, (char *)&superblock) < 0) { return -1; }

    // Initialize directory
    memset(root_dir, 0, sizeof(root_dir));
    if (write_block(superblock.root_dir_block, (char *)root_dir) < 0) { return -1; }

    // Close disk
    close_disk();
    return 0;
}

int mount_fs(char *disk_name) {
    if (fs_mounted) {
        fprintf(stderr, "Filesystem already mounted.\n");
        return -1;
    }

    if (open_disk(disk_name) < 0) {
        fprintf(stderr, "Error opening disk.\n");
        return -1;
    }

    char buffer[BLOCK_SIZE];

    // Read the superblock
    if (read_block(0, buffer) < 0) {
        fprintf(stderr, "Error reading superblock.\n");
        close_disk();
        return -1;
    }
    
    memcpy(&superblock, buffer, sizeof(superblock_t));

    // Check for a valid filesystem magic number
    if (superblock.magic != FS_MAGIC) {
        fprintf(stderr, "Invalid filesystem magic number.\n");
        close_disk();
        return -1;
    }

    // Load the root directory
    if (read_block(superblock.root_dir_block, (char *)root_dir) < 0) {
        fprintf(stderr, "Error reading root directory.\n");
        close_disk();
        return -1;
    }

    fs_mounted = 1;
    return 0;
}

int umount_fs(char *disk_name) {
    if (!fs_mounted) {
        fprintf(stderr, "No filesystem is currently mounted.\n");
        return -1;
    }

    char buffer[BLOCK_SIZE];

    // Write the superblock back to disk
    memcpy(buffer, &superblock, sizeof(superblock_t));
    if (write_block(0, buffer) < 0) {
        fprintf(stderr, "Error writing superblock.\n");
        return -1;
    }

    // Write root directory back to disk
    if (write_block(superblock.root_dir_block, (char *)root_dir) < 0) {
        fprintf(stderr, "Error writing root directory.\n");
        return -1;
    }
    
    if (close_disk() < 0) {
        fprintf(stderr, "Error closing disk.\n");
        return -1;
    }

    fs_mounted = 0;
    return 0;
}

int fs_format()
{
    memset(&superblock, 0, sizeof(superblock_t));
    superblock.magic = FS_MAGIC;

    char zero[SECTOR_SIZE] = {0};
    for (int i = 1; i < 1024; ++i)
    {
        write_block(i, zero);
    }

    return write_block(0, (char *)&superblock);
}

int fs_create(const char *name)
{
    if (!name || strlen(name) >= MAX_FILENAME_LEN)
        return -1;

    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (superblock.files[i].in_use && strcmp(superblock.files[i].name, name) == 0)
            return -1;
    }

    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (!superblock.files[i].in_use)
        {
            strcpy(superblock.files[i].name, name);
            superblock.files[i].size = 0;
            superblock.files[i].start_sector = -1;
            superblock.files[i].in_use = 1;
            return write_block(0, (char *)&superblock);
        }
    }

    return -1;
}

int fs_delete(const char *name)
{
    for (int i = 0; i < MAX_FILES; ++i) {
        if (superblock.files[i].in_use && strcmp(superblock.files[i].name, name) == 0){
            superblock.files[i].in_use = 0;
            return write_block(0, (char *)&superblock);
        }
    }
    return -1;
}

int fs_list()
{
    for (int i = 0; i < MAX_FILES; ++i) {
        if (superblock.files[i].in_use) {
            printf("%s\n", superblock.files[i].name);
        }
    }
    return 0;
}