// disc.c
#include "disk.h"
#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *disk_file = NULL; // File pointer for disk file

// Creates a new virtual disk
int make_disk(const char *name) {
    if (disk_file != NULL) {
        fprintf(stderr, "A disk is already open.\n");
        return -1;
    }

    // Create new disk (write binary mode)
    disk_file = fopen(name, "wb");
    if (disk_file == NULL) {
        perror("Error creating disk\n");
        return -1;
    }

    // Initialize the disk with empty blocks
    char empty_block[BLOCK_SIZE] = {0};
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if(fwrite(empty_block, sizeof(char), BLOCK_SIZE, disk_file) != BLOCK_SIZE) {
            perror("Error initializing disk.\n");
            fclose(disk_file);
            disk_file = NULL;
            return -1;
        }
    }

    fclose(disk_file);
    disk_file = NULL;
    printf("Disk created successfully: %s\n", name);
    return 0;
}

int open_disk(const char *name) {
    if (disk_file != NULL) {
        fprintf(stderr, "A disk is already open.\n");
        return -1;
    }

    // Open the disk for read and write
    disk_file = fopen(name, "r+b"); 

    if (disk_file == NULL) {
        perror("Error opening disk");
        return -1;
    }

    printf("Disk opened successfully: %s\n", name);
    return 0;
}

// Closes virtual disk
int close_disk() {
    if (disk_file == NULL) {
        fprintf(stderr, "Disk is not open.\n");
        return -1;
    }

    fclose(disk_file);
    disk_file = NULL;
    printf("Disk closed successfully.\n");
    return 0;
}

int read_block(int block, char *buffer) {
    if (disk_file == NULL) {
        fprintf(stderr, "Disk is not open!\n");
        return -1;
    }

    if (block < 0 || block >= MAX_BLOCKS) {
        fprintf(stderr, "Invalid block number: %d\n", block);
        return -1;
    }

    if (buffer == NULL) {
        fprintf(stderr, "Buffer is NULL. Cannot read from block %d.\n", block);
        return -1;
    }

    if (fseek(disk_file, block * BLOCK_SIZE, SEEK_SET) != 0) {
        perror("Error seeking block\n");
        return -1;
    }

    size_t read_bytes = fread(buffer, sizeof(char), BLOCK_SIZE, disk_file);
    if (read_bytes != BLOCK_SIZE) {
        perror("Error reading from block");
        fprintf(stderr, "Expected to read %d bytes, but read %lu bytes.\n", BLOCK_SIZE, read_bytes);
        return -1;
    }

    printf("Successfully read %lu bytes from block %d.\n", read_bytes, block);
    return 0;
}

int write_block(int block, const char *buffer) {
    if (disk_file == NULL) {
        fprintf(stderr, "Disk is not open!\n");
        return -1;
    }

    if (block < 0 || block >= MAX_BLOCKS) {
        fprintf(stderr, "Invalid block number: %d\n", block);
        return -1;
    }

    if (buffer == NULL) {
        fprintf(stderr, "Buffer is NULL. Cannot write to block %d.\n", block);
        return -1;
    }
       
    if (fseek(disk_file, block * BLOCK_SIZE, SEEK_SET) != 0) {
        perror("Error seeking block\n");
        return -1;
    }

    size_t written = fwrite(buffer, sizeof(char), BLOCK_SIZE, disk_file);
    if (written != BLOCK_SIZE) {
        perror("Error writing to block");
        fprintf(stderr, "Expected to write %d bytes, but wrote %lu bytes.\n", BLOCK_SIZE, written);
        return -1;
    }
    
    fflush(disk_file);
    printf("Successfully wrote %lu bytes to block %d.\n", written, block);
    return 0;
}


