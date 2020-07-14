#include "vfs.h"

#include "fatfs.h"
#include "io.h"
#include "pool.h"
#include "sdhost.h"
#include "tmpfs.h"
#include "utils.h"
struct mount* rootfs;
struct filesystem tmpfs, fatfs;
struct vnode* currentdir;

void init_rootfs() {
    rootfs = kmalloc(sizeof(struct mount));
    fatfs.name = "fatfs";
    register_filesystem(&fatfs);
    fatfs.setup_mount(&fatfs, rootfs);
    currentdir = rootfs->root;
}

int register_filesystem(struct filesystem* fs) {
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.
    if (!strcmp(fs->name, "tmpfs")) {
        fs->setup_mount = tmpfs_mount;
        tmpfs_v_ops = kmalloc(sizeof(struct file_operations));
        tmpfs_f_ops = kmalloc(sizeof(struct file_operations));
        tmpfs_v_ops->lookup = tmpfs_lookup;
        tmpfs_v_ops->create = tmpfs_create;
        tmpfs_v_ops->chdir = tmpfs_chdir;
        tmpfs_f_ops->write = tmpfs_write;
        tmpfs_f_ops->read = tmpfs_read;
        tmpfs_f_ops->list = tmpfs_list;
        return 1;
    } else if (!strcmp(fs->name, "fatfs")) {
        asm volatile("fatf:");
        sd_init();
        fs->setup_mount = fatfs_mount;
        fatfs_v_ops = kmalloc(sizeof(struct file_operations));
        fatfs_f_ops = kmalloc(sizeof(struct file_operations));
        fatfs_v_ops->lookup = fatfs_lookup;
        fatfs_f_ops->write = fatfs_write;
        fatfs_f_ops->read = fatfs_read;
        // fatfs_f_ops->list = fatfs_list;
        return 1;
    }
    return -1;
}

struct file* vfs_open(const char* pathname, int flags) {
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.
    struct vnode* target;
    struct file* fd = 0;
    if (!strcmp(pathname, "/") && flags != O_CREAT) {
        target = rootfs->root;
        fd = (struct file*)kmalloc(sizeof(struct file));
        fd->vnode = target;
        fd->f_ops = target->f_ops;
        fd->f_pos = 0;
    } else if (!strcmp(pathname, ".") && flags != O_CREAT) {
        target = currentdir;
        fd = (struct file*)kmalloc(sizeof(struct file));
        fd->vnode = target;
        fd->f_ops = target->f_ops;
        fd->f_pos = 0;
    } else if (flags == O_CREAT) {
        int ret = currentdir->v_ops->lookup(currentdir, &target, pathname);
        if (ret == -1) {
            currentdir->v_ops->create(currentdir, &target, pathname);
            fd = (struct file*)kmalloc(sizeof(struct file));
            fd->vnode = target;
            fd->f_ops = target->f_ops;
            fd->f_pos = 0;
        } else {
            print_s("File exist!!\n");
        }
    } else {
        int ret = currentdir->v_ops->lookup(currentdir, &target, pathname);
        if (ret == -1) {
            print_s("File not found!!\n");
        } else {
            fd = (struct file*)kmalloc(sizeof(struct file));
            fd->vnode = target;
            fd->f_ops = target->f_ops;
            fd->f_pos = 0;
        }
    }
    return fd;
}

int vfs_close(struct file* file) {
    print_s("Close fd!!\n");
    kfree(file);
    return 1;
}

int vfs_list(struct file* file) {
    int ret = file->f_ops->list(file);
    return ret;
}

int vfs_write(struct file* file, const void* buf, size_t len) {
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
    return file->f_ops->write(file, buf, len);
}

int vfs_read(struct file* file, void* buf, size_t len) {
    // 1. read min(len, readable file data size) byte to buf from the opened
    // file.
    // 2. return read size or error code if an error occurs.
    return file->f_ops->read(file, buf, len);
}

int mkdir(const char* pathname) { return tmpfs_mkdir(currentdir, pathname); }

int chdir(const char* pathname) {
    struct vnode* target;
    int ret = currentdir->v_ops->chdir(currentdir, &target, pathname);
    currentdir = target;
    return ret;
}
