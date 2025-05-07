int create_disk(char *name);
int open_disk(char *name);
int close_disk();
int block_write(int block, char *buf);
int block_read(int block, char *buf);
