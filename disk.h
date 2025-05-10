// disk.h
#ifndef DISK_H
#define DISK_H

// Disk Operations
int make_disk(const char *name);
int open_disk(const char *name);
int close_disk();

int read_block(int block, char *buffer);
int write_block(int block, const char *buffer);

#endif // DISK_H