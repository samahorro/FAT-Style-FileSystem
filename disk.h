// disk.h
#ifndef DISK_H
#define DISK_H

int make_disk(const char *name, int n);
int open_disk(const char *name, int n);
int read_sector(int sector, char *buffer);
int write_sector(int sector, const char *buffer);
int close_disk();

#endif // DISK_H