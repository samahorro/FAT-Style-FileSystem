// disk.h
#ifndef DISK_H
#define DISK_H

#include <stddef.h>

// Disk functions prototypes
int make_disk(const char *name, int n); // Add the 'n' parameter
int open_disk(const char *name, int n); // Add the 'n' parameter
int close_disk(void);
int read_block(int block, char *buffer);
int write_block(int block, const char *buffer);

#endif // DISK_H
