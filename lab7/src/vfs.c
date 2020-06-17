#include "vfs.h"


int register_filesystem(filesystem_t *fs)
{
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.
    printf("Mount filesystem name is %s\n", fs->name);
    return fs->setup_mount(fs, &rootfs);
}

file_t *vfs_open(const char *pathname, int flags)
{
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.
}
int vfs_close(file_t *file)
{
    // 1. release the file descriptor
}
int vfs_write(file_t *file, const void *buf, size_t len)
{
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
}
int vfs_read(file_t *file, void *buf, size_t len)
{
    // 1. read min(len, readable file data size) byte to buf from the opened file.
    // 2. return read size or error code if an error occurs.
}