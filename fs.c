// fs.c
#include "fs.h"
#include "disk.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILES 128
#define MAX_FILENAME 32
#define MAX_FILE_SIZE 8192
#define SECTOR_SIZE 512
#define FS_MAGIC 0x12345678

typedef struct
{
    char name[MAX_FILENAME];
    int size;
    int start_sector;
    int in_use;
} FileEntry;

typedef struct
{
    int magic;
    FileEntry files[MAX_FILES];
    char bitmap[SECTOR_SIZE];
} SuperBlock;

static SuperBlock superblock;

int fs_format()
{
    memset(&superblock, 0, sizeof(SuperBlock));
    superblock.magic = FS_MAGIC;

    char zero[SECTOR_SIZE] = {0};
    for (int i = 1; i < 1024; ++i)
    {
        write_sector(i, zero);
    }

    return write_sector(0, (char *)&superblock);
}

int fs_mount()
{
    if (read_sector(0, (char *)&superblock) < 0 || superblock.magic != FS_MAGIC)
        return -1;
    return 0;
}

int fs_create(const char *name)
{
    if (!name || strlen(name) >= MAX_FILENAME)
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
            return write_sector(0, (char *)&superblock);
        }
    }

    return -1;
}

int fs_delete(const char *name)
{
    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (superblock.files[i].in_use && strcmp(superblock.files[i].name, name) == 0)
        {
            superblock.files[i].in_use = 0;
            return write_sector(0, (char *)&superblock);
        }
    }
    return -1;
}

int fs_list()
{
    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (superblock.files[i].in_use)
            printf("%s\n", superblock.files[i].name);
    }
    return 0;
}