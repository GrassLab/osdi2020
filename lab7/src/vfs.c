#include "vfs.h"

mount *rootfs;

#define FILE_ARR_SIZE 16

file file_arr[FILE_ARR_SIZE];

int register_filesystem(filesystem *fs)
{
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.
    uart_puts("Mount filesystem ");
    uart_puts(fs->name);
    uart_puts("\n");

    for(int i=0;i<FILE_ARR_SIZE;i++){
        file_arr[i].vnode = NULL;
    }
    return fs->setup_mount(fs, &rootfs);
}
void find_null(file** ptr, vnode *target){
    for (int i = 0; i < FILE_ARR_SIZE; i++)
        if (file_arr[i].vnode == NULL){
            *ptr = &file_arr[i];
            (*ptr)->vnode = target;
            (*ptr)->f_ops = target->f_ops;
            (*ptr)->f_pos = 0;

            uart_puts("file vnode ");
            uart_hex((*ptr)->vnode);
            uart_puts(" on ");
            uart_send_int(i);
            uart_puts("\n");
            return;
        }
    
    uart_puts("file array full!\n");
    return;
}
file *vfs_open(const char *pathname, int flags)
{
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.
    vnode *target;
    file *filee = NULL;
    int ret_val;

    
    if (flags == O_CREAT){
        uart_puts("vfs open O_CREAT\n");
        ret_val = rootfs->root->v_ops->lookup(rootfs->root, &target, pathname);

        if (ret_val != -1)
            uart_puts("File exist, can't create file\n");
        else{
            ret_val = rootfs->root->v_ops->create(rootfs->root, &target, pathname);
            find_null(&filee, target);
            // assert(filee!=NULL);
        }
        
        return filee;
    }
    else{
        uart_puts("vfs open O_OPEN\n");
        ret_val = rootfs->root->v_ops->lookup(rootfs->root, &target, pathname);
        if (ret_val >= 0)
            find_null(&filee, target);
        else
            uart_puts("return NULL\n");
        
        return filee;
    }
}
int vfs_close(file *filee)
{
    // 1. release the file descriptor
    uart_puts("vfs close\n");
    for (int i = 0; i < FILE_ARR_SIZE; i++)
        if (filee == &file_arr[i]){
            filee->vnode = NULL;
            uart_puts("vfs close succeeded on ");
            uart_send_int(i);
            uart_puts("\n");
            return 0;
        }
    
    uart_puts("vfs close failed\n");
    return -1;
}

int vfs_write(file *file, const void *buf, size_t len){
    uart_puts("vfs write\n");
    int ret_val = file->vnode->f_ops->write(file, buf, len);
    if (ret_val >= 0)
        uart_puts("vfs write succeeded\n");
    else
        uart_puts("vfs write failed\n");
    
    return ret_val;
}
int vfs_read(file *file, void *buf, size_t len){
    uart_puts("vfs read\n");
    int ret_val = file->f_ops->read(file, buf, len);
    if (ret_val >= 0)
        uart_puts("vfs read succeeded\n");
    else
        uart_puts("vfs read failed\n");
    
    return ret_val;
}
