#include "disk.h"
#include "fs.h"
#include <stdio.h>
#include <stdlib.h>

static FILE *disk_file = NULL; // File pointer for disk file

// Creates a new virtual disk
int make_disk(const char *name) {
    disk_file = fopen(name, "wb"); // Create new disk (write binary mode)
    if (disk_file == NULL)
    {
        perror("Error creating disk");
        return -1;
    }

    // Initialize the disk with empty blocks
    char empty_block[BLOCK_SIZE] = {0};
    for (int i = 0; i < MAX_BLOCKS; i++) {
        fwrite(empty_block, sizeof(char), BLOCK_SIZE, disk_file);
    }

    fclose(disk_file);
    return 0;
}

int open_disk(const char *name) {
    disk_file = fopen(name, "r+b"); // Open the disk for read and write
    if (disk_file == NULL)
    {
        perror("Error opening disk");
        return -1;
    }
    return 0;
}

int close_disk() {
    if (disk_file != NULL) {
        fclose(disk_file);
        disk_file = NULL;
        return 0;
    }
    return -1;
}

int read_block(int block, char *buffer) {
    if (disk_file == NULL) {
        fprintf(stderr, "Disk is not open!\n");
        return -1;
    }

    fseek(disk_file, block * BLOCK_SIZE, SEEK_SET);
    size_t read_bytes = fread(buffer, sizeof(char), BLOCK_SIZE, disk_file);

    if (read_bytes != BLOCK_SIZE) {
        perror("Error reading from block");
        return -1;
    }
    return 0;
}

int write_block(int block, const char *buffer) {
    if (disk_file == NULL) {
        fprintf(stderr, "Disk is not open!\n");
        return -1;
    }

    fseek(disk_file, block * BLOCK_SIZE, SEEK_SET);
    size_t written = fwrite(buffer, sizeof(char), BLOCK_SIZE, disk_file);
    
    if (written != BLOCK_SIZE) {
        perror("Error writing to block");
        return -1;
    }
    return 0;
}


