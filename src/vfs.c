#include "vfs.h"

#include "allocator.h"
#include "string.h"

mount_t * rootfs;

int register_filesystem ( struct filesystem * fs )
{
    if ( !strcmp ( fs->name, "tmpfs" ) )
    {
        rootfs = (mount_t *) kmalloc ( sizeof ( mount_t ) );

        fs->setup_mount ( fs, rootfs );

        return 1;
    }
    // other type of file system would be put on another global variable

    return 0;
}

// struct file * vfs_open ( const char * pathname, int flags )
// {
//     // 1. Lookup pathname from the root vnode.
//     // 2. Create a new file descriptor for this vnode if found.
//     // 3. Create a new file if O_CREAT is specified in flags.
// }
// int vfs_close ( struct file * file )
// {
//     // 1. release the file descriptor
// }
// int vfs_write ( struct file * file, const void * buf, size_t len )
// {
//     // 1. write len byte from buf to the opened file.
//     // 2. return written size or error code if an error occurs.
// }
// int vfs_read ( struct file * file, void * buf, size_t len )
// {
//     // 1. read min(len, readable file data size) byte to buf from the opened file.
//     // 2. return read size or error code if an error occurs.
// }