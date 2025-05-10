#include "fs.h"
#include "disk.h"
#include <stdio.h>

int main() {
    make_fs("disk42");
    mount_fs("disk42");

    printf("Creating files...\n");
    fs_create("file1");
    fs_create("file2");
    fs_create("file3");

    umount_fs("disk42");

    return 0;
}

// COMPILE WITH "gcc -o fs fs.c disk.c main.c"
