#include "vfs.h"
#include "uart.h"

mount_t* rootfs;
file_t file_table[FILE_TABLE_SIZE];

int register_filesystem(filesystem_t* fs)
{
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.
    uart_puts("mount fs ");
    uart_puts(fs->name);
    uart_puts("\r\n");

    // initialize file table
    for (int i = 0; i < FILE_TABLE_SIZE; i++) {
        file_table[i].vnode = 0;
    }

    return fs->setup_mount(fs, &rootfs);
}

file_t* vfs_open(const char* pathname, int flags)
{
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.
    vnode_t* target = 0;
    file_t* file = 0;

    uart_puts("vfs_open\r\n");
    if (flags == O_CREAT) {
        int lookup_ret = rootfs->root->v_ops->lookup(rootfs->root, &target, pathname);

        if (lookup_ret != -1) {
            uart_puts(pathname);
            uart_puts(" file exists.\r\n");
        } else {
            int create_ret = rootfs->root->v_ops->create(rootfs->root, &target, pathname);

            for (int i = 0; i < FILE_TABLE_SIZE; i++) {
                if (file_table[i].vnode == 0) {
                    file = &file_table[i];
                    file->vnode = target;
                    file->f_ops = target->f_ops;
                    file->f_pos = 0;

                    uart_puts("file vnode 0x");
                    uart_hex(file->vnode);
                    uart_puts("\r\n");
                    break;
                }
            }
        }
    } else if (flags == O_OPEN) {
        int lookup_ret = rootfs->root->v_ops->lookup(rootfs->root, &target, pathname);
        if (lookup_ret >= 0) {
            for (int i = 0; i < FILE_TABLE_SIZE; i++) {
                if (file_table[i].vnode == 0) {
                    file = &file_table[i];
                    file->vnode = target;
                    file->f_ops = target->f_ops;
                    file->f_pos = 0;
                    uart_puts("file open\r\n");
                    break;
                }
            }
        } else {
            uart_puts("file does not exist.\r\n");
        }
    }
    return file;
}

int vfs_close(file_t* file)
{
    uart_puts("close file 0x");
    uart_hex(file->vnode);
    uart_puts("\r\n");

    for (int i = 0; i < FILE_TABLE_SIZE; i++) {
        if (file == &file_table[i]) {
            file->vnode = 0;
            uart_puts("vfs close success.\r\n");
            return SUCCESS;
        }
    }
    uart_puts("vfs close fail.\r\n");
    return FAIL;
}

int vfs_write(file_t* file, const void* buf, unsigned long len)
{
    uart_puts("vfs write\r\n");

    int write_ret = file->vnode->f_ops->write(file, buf, len);
    if (write_ret >= 0)
        uart_puts("vfs write success\r\n");
    else
        uart_puts("vfs write fail\r\n");

    return write_ret;
}

int vfs_read(file_t* file, void* buf, unsigned long len)
{
    uart_puts("vfs read\r\n");

    int read_ret = file->f_ops->read(file, buf, len);
    if (read_ret >= 0)
        uart_puts("vfs read success\r\n");
    else
        uart_puts("vfs read fail\r\n");

    return read_ret;
}