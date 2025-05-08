#ifndef DISK_H
#define DISK_H

int make_disk(char *name); // Or create_disk(), pick one
int open_disk(char *name);
int close_disk();
int block_write(int block, char *buf);
int block_read(int block, char *buf);

#endif
