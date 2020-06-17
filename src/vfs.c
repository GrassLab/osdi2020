#include "vfs.h"

#include "mm.h"
#include "my_string.h"
#include "tmpfs.h"
#include "uart0.h"
#include "util.h"
#include "schedule.h"

struct mount* rootfs;

void rootfs_init() {
    struct filesystem* tmpfs = (struct filesystem*)kmalloc(sizeof(struct filesystem));
    tmpfs->name = (char*)kmalloc(sizeof(char) * 6);
    strcpy(tmpfs->name, "tmpfs");
    tmpfs->setup_mount = tmpfs_setup_mount;
    register_filesystem(tmpfs);

    rootfs = (struct mount*)kmalloc(sizeof(struct mount));
    tmpfs->setup_mount(tmpfs, rootfs);
}

int register_filesystem(struct filesystem* fs) {
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.
    if (!strcmp(fs->name, "tmpfs")) {
        uart_printf("\n[%f] Register tmpfs", get_timestamp());
        return tmpfs_register();
    }
    return -1;
}

struct file* create_fd(struct vnode* target) {
    struct file* fd = (struct file*)kmalloc(sizeof(struct file));
    fd->f_ops = target->f_ops;
    fd->vnode = target;
    fd->f_pos = 0;
    return fd;
}

void traversal(const char* pathname, struct vnode** target_node, char* target_path) {
    if (pathname[0] == '/') {  // absolute path
        *target_node = rootfs->root->vnode;
        strcpy(target_path, pathname + 1);
    }
    else {  // relative path
        *target_node = current_task->pwd->vnode;
        strcpy(target_path, pathname);
    }
}

struct file* vfs_open(const char* pathname, int flags) {
    // 1. Find target_dir node and target_path based on pathname
    struct vnode* target_dir;
    char target_path[128];
    traversal(pathname, &target_dir, target_path);
    // 2. Create a new file descriptor for this vnode if found.
    struct vnode* target_file;
    if (rootfs->root->vnode->v_ops->lookup(target_dir, &target_file, target_path) == 0) {
        return create_fd(target_file);
    }
    // 3. Create a new file if O_CREAT is specified in flags.
    else {
        if (flags & O_CREAT) {
            rootfs->root->vnode->v_ops->create(target_dir, &target_file, target_path);
            return create_fd(target_file);
        }
        else {
            return NULL;
        }
    }
}

int vfs_close(struct file* file) {
    // 1. release the file descriptor
    kfree((void*)file);
    return 0;
}

int vfs_write(struct file* file, const void* buf, uint64_t len) {
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
    return file->f_ops->write(file, buf, len);
}

int vfs_read(struct file* file, void* buf, uint64_t len) {
    // 1. read min(len, readable file data size) byte to buf from the opened file.
    // 2. return read size or error code if an error occurs.
    return file->f_ops->read(file, buf, len);
}

int vfs_readdir(struct file* fd) {
    return fd->vnode->v_ops->ls(fd->vnode);
}

int vfs_mkdir(const char* pathname) {
    struct vnode* target_dir;
    char child_name[128];
    traversal(pathname, &target_dir, child_name);
    struct vnode* child_dir;
    return target_dir->v_ops->mkdir(target_dir, &child_dir, child_name);
}

int vfs_chdir(const char* pathname) {

}

int vfs_mount(const char* device, const char* mountpoint, const char* filesystem) {

}

int vfs_umount(const char* mountpoint) {

}