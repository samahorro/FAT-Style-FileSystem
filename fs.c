#include "disk.h"
#include "fs.h"
#include <string.h>

SuperBlock sb;
int fat[BLOCK_COUNT];
DirectoryEntry dir[MAX_FILES];
FileDescriptor fd_table[MAX_FD];

// block below will create a disk and then initialize it.
int make_(char *name) {
    if (make_disk(name) == -1) return -1;
    if (open_disk(name) == -1) return -1;

    sb.fat_start = 1;
    sb.dir_start = 17;
    sb.data_start = 81;

    for (int i = 0; i < BLOCK_COUNT; i++) fat[i] = -1;
    for (int i = 0; i < MAX_FILES; i++) dir[i].used = 0;

    char buf[BLOCK_SIZE];
    memcpy(buf, &sb, sizeof(sb));
    block_write(0, buf);

    for (int i = 0; i < 16; i++) {
        memcpy(buf, fat + i * 1024, BLOCK_SIZE);
        block_write(sb.fat_start + i, buf);
    }

    for (int i = 0; i < 4; i++) {
        memcpy(buf, dir + i * 16, BLOCK_SIZE);
        block_write(sb.dir_start + i, buf);
    }

    close_disk();
    return 0;
}

// mount to load disk into memory

// umount_ to save memory onto disk
