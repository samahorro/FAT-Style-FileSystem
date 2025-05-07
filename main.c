#include "fs.h"

int main() {
    make("disk42");
    mount("disk42");
    umount("disk42");
    return 0;
}

// COMPILE WITH "gcc -o fs fs.c disk.c main.c"
