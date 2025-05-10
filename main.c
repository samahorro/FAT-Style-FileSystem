#include <stdio.h>
#include <string.h>
#include "fs.h"

int main()
{
    // Define the disk name
    const char *disk_name = "virtual_disk.img";

    // Step 1: Create a new disk and initialize a filesystem
    printf("Creating disk...\n");
    if (make_fs(disk_name) != 0)
    {
        fprintf(stderr, "Failed to create filesystem.\n");
        return 1;
    }
    printf("Filesystem created successfully.\n");

    // Step 2: Mount the filesystem
    printf("Mounting filesystem...\n");
    if (mount_fs(disk_name) != 0)
    {
        fprintf(stderr, "Failed to mount filesystem.\n");
        return 1;
    }
    printf("Filesystem mounted successfully.\n");

    // Step 3: Create files
    printf("Creating file 'file1.txt'...\n");
    if (fs_create("file1.txt") != 0)
    {
        fprintf(stderr, "Failed to create 'file1.txt'.\n");
        return 1;
    }

    printf("Creating file 'file2.txt'...\n");
    if (fs_create("file2.txt") != 0)
    {
        fprintf(stderr, "Failed to create 'file2.txt'.\n");
        return 1;
    }

    // Step 4: List files
    printf("Listing files:\n");
    fs_list();

    // Step 5: Delete a file
    printf("Deleting file 'file1.txt'...\n");
    if (fs_delete("file1.txt") != 0)
    {
        fprintf(stderr, "Failed to delete 'file1.txt'.\n");
        return 1;
    }

    // Step 6: List files again after deletion
    printf("Listing files after deletion:\n");
    fs_list();

    // Step 7: Unmount the filesystem
    printf("Unmounting filesystem...\n");
    if (umount_fs(disk_name) != 0)
    {
        fprintf(stderr, "Failed to unmount filesystem.\n");
        return 1;
    }
    printf("Filesystem unmounted successfully.\n");

    // Final message
    printf("Filesystem operations completed successfully.\n");
    return 0;
}
