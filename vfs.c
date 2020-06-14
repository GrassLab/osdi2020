#include "vfs.h"

mount_t *rootfs;

#define FILE_ARR_SIZE 16

file_t file_arr[FILE_ARR_SIZE];
int file_arr_head = 0;

int register_filesystem(filesystem_t *fs)
{
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.
    printf("Mount filesystem %s\n", fs->name);

    return fs->setup_mount(fs, &rootfs);
}

file_t *vfs_open(const char *pathname, int flags)
{
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.
    vnode_t *target;
    file_t *file = NULL;
    int ret_val;

    printf("vfs open\n");
    if (flags == O_CREAT)
    {
        ret_val = rootfs->root->v_ops->lookup(rootfs->root, &target, pathname);

        if (ret_val != -1)
        {
            printf("File exist, can't create file\n");
        }
        else
        {
            ret_val = rootfs->root->v_ops->create(rootfs->root, &target, pathname);
            for (int i = 0; i < FILE_ARR_SIZE; i++)
            {
                if (file_arr[i].vnode == NULL)
                {
                    file = &file_arr[i];
                    file->vnode = target;
                    file->f_ops = target->f_ops;
                    file->f_pos = 0;

                    printf("file vnode %x\n", file->vnode);
                    break;
                }
            }
        }

        return file;
    }
    else
    {
        ret_val = rootfs->root->v_ops->lookup(rootfs->root, &target, pathname);
        if (ret_val >= 0)
        {
            for (int i = 0; i < FILE_ARR_SIZE; i++)
            {
                if (file_arr[i].vnode == NULL)
                {
                    file = &file_arr[i];
                    file->vnode = target;
                    file->f_ops = target->f_ops;
                    file->f_pos = 0;
                    break;
                }
            }
        }

        return file;
    }
}
int vfs_close(file_t *file)
{
    // 1. release the file descriptor
    printf("vfs close\n");
    for (int i = 0; i < FILE_ARR_SIZE; i++)
    {
        if (file == &file_arr[i])
        {
            file->vnode = NULL;
            printf("vfs close succeeded\n");
            return 0;
        }
    }
    printf("vfs close failed\n");
    return -1;
}
int vfs_write(file_t *file, const void *buf, size_t len)
{
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
    printf("vfs write\n");
    int ret_val = file->vnode->f_ops->write(file, buf, len);
    if (ret_val >= 0)
    {
        printf("vfs write succeeded\n");
    }
    else
    {
        printf("vfs write failed\n");
    }
    return ret_val;
}
int vfs_read(file_t *file, void *buf, size_t len)
{
    // 1. read min(len, readable file data size) byte to buf from the opened file.
    // 2. return read size or error code if an error occurs.
    printf("vfs read\n");
    int ret_val = file->f_ops->read(file, buf, len);
    if (ret_val >= 0)
    {
        printf("vfs read succeeded\n");
    }
    else
    {
        printf("vfs read failed\n");
    }
    return ret_val;
}
