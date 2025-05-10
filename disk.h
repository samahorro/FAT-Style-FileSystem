// disk.h
#ifndef DISK_H
#define DISK_H

// Disk Operations
int make_disk(const char *name, int n);
int open_disk(const char *name, int n);
int close_disk();

int read_block(int sector, char *buffer);
int write_block(int sector, const char *buffer);

#endif // DISK_H