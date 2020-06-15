#include "vfs.h"

mount *rootfs;

#define FILE_ARR_SIZE 16

file file_arr[FILE_ARR_SIZE];
int file_arr_head = 0;

int register_filesystem(filesystem *fs)
{
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.
    uart_puts("Mount filesystem ");
    uart_puts(fs->name);
    uart_puts("\n");

    return fs->setup_mount(fs, &rootfs);
}

file *vfs_open(const char *pathname, int flags)
{
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.
    vnode *target;
    file *filee = NULL;
    int ret_val;

    
    if (flags == O_CREAT)
    {
        uart_puts("vfs open O_CREAT\n");
        ret_val = rootfs->root->v_ops->lookup(rootfs->root, &target, pathname);

        if (ret_val != -1)
        {
            uart_puts("File exist, can't create file\n");
        }
        else
        {
            ret_val = rootfs->root->v_ops->create(rootfs->root, &target, pathname);
            for (int i = 0; i < FILE_ARR_SIZE; i++)
            {
                if (file_arr[i].vnode == NULL)
                {
                    filee = &file_arr[i];
                    filee->vnode = target;
                    filee->f_ops = target->f_ops;
                    filee->f_pos = 0;

                    uart_puts("file vnode ");
                    uart_hex(filee->vnode);
                    uart_puts("\n");
                    break;
                }
            }
        }

        return filee;
    }
    else
    {
        uart_puts("vfs open O_OPEN\n");
        ret_val = rootfs->root->v_ops->lookup(rootfs->root, &target, pathname);
        if (ret_val >= 0)
        {
            for (int i = 0; i < FILE_ARR_SIZE; i++)
            {
                if (file_arr[i].vnode == NULL)
                {
                    filee = &file_arr[i];
                    filee->vnode = target;
                    filee->f_ops = target->f_ops;
                    filee->f_pos = 0;

                    uart_puts("file vnode ");
                    uart_hex(filee->vnode);
                    uart_puts("\n");
                    break;
                }
            }
        }
        uart_puts("return NULL\n");
        
        return filee;
    }
}
int vfs_close(file *filee)
{
    // 1. release the file descriptor
    uart_puts("vfs close\n");
    for (int i = 0; i < FILE_ARR_SIZE; i++)
    {
        if (filee == &file_arr[i])
        {
            filee->vnode = NULL;
            uart_puts("vfs close succeeded\n");
            return 0;
        }
    }
    uart_puts("vfs close failed\n");
    return -1;
}