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
    memset(&superblock, 0, sizeof(superblock_t));
    superblock.root_dir_block = 1;
    superblock.fat_block = 2;
    superblock.free_block_count = MAX_BLOCKS - 2;
    superblock.magic = FS_MAGIC;
    
    // Write initialized superblock back to disk
    char buffer[BLOCK_SIZE] = {0};
    memcpy(buffer, &superblock, sizeof(superblock_t));

    if (write_block(0, buffer) < 0) { 
        fprintf(stderr, "Error writing superblock.\n");
        close_disk();
        return -1;
    }

    // Initialize directory and debug printf statement
    char root_buffer[BLOCK_SIZE] = {0};
    memcpy(root_buffer, root_dir, sizeof(root_dir));
    printf("Root directory buffer prepared. Buffer size: %lu bytes\n", sizeof(root_buffer));

    if (write_block(1, root_buffer) < 0) { 
        fprintf(stderr, "Error writing root directory.\n");
        close_disk();
        return -1; 
    }

    // Close disk
    close_disk();
    printf("Filesystem created successfully.\n");
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

    // Copy buffer data to superblock
    memcpy(&superblock, buffer, sizeof(superblock_t));
    
    // Check for a valid filesystem magic number
    if (superblock.magic != FS_MAGIC) {
        fprintf(stderr, "Invalid filesystem magic number: %x\n", superblock.magic);
        close_disk();
        return -1;
    }

    // Debug print statement for superblock being loaded
    printf("Superblock loaded. Free blocks: %d\n", superblock.free_block_count);

    // Read the root directory
    char root_buffer[BLOCK_SIZE] = {0};
    if (read_block(1, root_buffer) < 0) {
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
    char root_buffer[BLOCK_SIZE] = {0};
    memcpy(root_buffer, root_dir, sizeof(root_dir));
    if (write_block(1, root_buffer) < 0) {
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

int fs_create(const char *name) {
    if (!fs_mounted) {
        fprintf(stderr, "Filesystem is not mounted.\n");
        return -1;
    }

    if (!name || strlen(name) >= MAX_FILENAME_LEN) { return -1; }

    for (int i = 0; i < MAX_FILES; ++i) {
        if (superblock.files[i].in_use && strcmp(superblock.files[i].name, name) == 0) {
            return -1;
        }
    }

    for (int i = 0; i < MAX_FILES; ++i) {
        if (!superblock.files[i].in_use) {
            strcpy(superblock.files[i].name, name);
            superblock.files[i].size = 0;
            superblock.files[i].start_sector = -1;
            superblock.files[i].in_use = 1;
            return write_block(0, (char *)&superblock);
        }
    }

    return -1;
}

int fs_delete(const char *name) {
    if (!fs_mounted) {
        fprintf(stderr, "Filesystem is not mounted.\n");
        return -1;
    }

    for (int i = 0; i < MAX_FILES; ++i) {
        if (superblock.files[i].in_use && strcmp(superblock.files[i].name, name) == 0){
            superblock.files[i].in_use = 0;
            return write_block(0, (char *)&superblock);
        }
    }
    return -1;
}

void fs_list_files() {
    for (int i = 0; i < MAX_FILES; ++i) {
        if (superblock.files[i].in_use) {
            printf("%s\n", superblock.files[i].name);
        }
    }
}