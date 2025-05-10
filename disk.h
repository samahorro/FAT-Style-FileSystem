// disk.c
#include "disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SECTOR_SIZE 4096

static FILE *disk_file = NULL;
static int n_sectors = 0;

int make_disk(const char *name, int n)
{
    if (n <= 0 || name == NULL)
        return -1;

    FILE *f = fopen(name, "w");
    if (!f)
        return -1;

    char zero = 0;
    for (int i = 0; i < n * SECTOR_SIZE; ++i)
    {
        fwrite(&zero, 1, 1, f);
    }

    fclose(f);
    return 0;
}

int open_disk(const char *name, int n)
{
    if (!name || n <= 0)
        return -1;

    if (disk_file)
        fclose(disk_file);

    disk_file = fopen(name, "r+b");
    if (!disk_file)
        return -1;

    n_sectors = n;
    return 0;
}

int read_block(int block, char *buffer)
{
    if (!disk_file || block < 0 || block >= n_sectors || !buffer)
        return -1;

    fseek(disk_file, block * SECTOR_SIZE, SEEK_SET);
    size_t result = fread(buffer, 1, SECTOR_SIZE, disk_file);
    return result == SECTOR_SIZE ? 0 : -1;
}

int write_block(int block, const char *buffer)
{
    if (!disk_file || block < 0 || block >= n_sectors || !buffer)
        return -1;

    fseek(disk_file, block * SECTOR_SIZE, SEEK_SET);
    size_t result = fwrite(buffer, 1, SECTOR_SIZE, disk_file);
    fflush(disk_file);
    return result == SECTOR_SIZE ? 0 : -1;
}

int close_disk()
{
    if (!disk_file)
        return -1;
    fclose(disk_file);
    disk_file = NULL;
    return 0;
}
