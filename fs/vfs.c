#include "vfs.h"
#include <stdio.h>
#include <stdlib.h>



struct mount* rootfs;
struct fs_manager FSManager; 

/*  
 *  Every malloc() can be replaced by symbol predefined in the linker script
 */

// int get_component_name(const char* pathname, char* component_name)
// {
//     printf("get component name of %s\n", pathname);
//     char* ptr = pathname;
//     while (!ptr) {
//         *component_name++ = *ptr++;
//         if (strncmp(&ptr, "/", 1) == 0) {
//             printf("ptr: %s\n", ptr);
//             return 0;
//         }
//     }
//     return 1;
// }

int register_filesystem(struct filesystem* fs) 
{
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.
    if (!FSManager.num) {
        FSManager.num = 0;
    }
    FSManager.recognize_fs[FSManager.num++] = fs;
    fs->setup_mount(fs, rootfs);
    printf("%s\n", rootfs->dentry->name);
    return 0;
}

struct file* vfs_open(const char* pathname, int flags) 
{
    log("vfs open.\n");
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.
    struct vnode* vnode;
    struct vnode* target;
    vnode = rootfs->root;
    // char component_name[NAME_LENGTH];
    // get_component_name(pathname, component_name);
    // printf("component_name: %s\n", component_name);
    int found = vnode->v_ops->lookup(rootfs->dentry, &target, pathname);
    // printf("found: %d\n", found);
    if (flags == O_CREAT) {
        if (found == 0) { // not found
            vnode->v_ops->create(rootfs->dentry, &target, pathname);
        } 
    } else {
        if (found == 0) { // not found
            return 0;
        } 
    }
    log("create fd.\n");
    struct file* fd = malloc(sizeof(struct file));
    fd->vnode = target;
    fd->f_ops = target->f_ops;
    fd->f_pos = 0;
    return fd;
}
int vfs_close(struct file* file) 
{
    // 1. release the file descriptor
    log("vfs close.\n");
    free(file);
    return 0;
}
int vfs_write(struct file* file, const void* buf, int len) 
{
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
    log("vfs write.\n");
    return file->f_ops->write(file, buf, len);
}
int vfs_read(struct file* file, void* buf, int len) 
{
    // 1. read min(len, readable file data size) byte to buf from the opened file.
    // 2. return read size or error code if an error occurs.
    log("vfs read.\n");
    return file->f_ops->read(file, buf, len);
}