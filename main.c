#include "fs.h"
#include "disk.h"

int main() {
    make_fs("disk42");
    mount_fs("disk42");
    umount_fs("disk42");
    return 0;
}

// COMPILE WITH "gcc -o fs fs.c disk.c main.c"
