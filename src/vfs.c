#include "vfs.h"

#include "mm.h"
#include "my_string.h"
#include "tmpfs.h"
#include "fat32.h"
#include "uart0.h"
#include "util.h"
#include "schedule.h"
#include "fs.h"

struct mount* rootfs;

void rootfs_init() {
    register_filesystem(&tmpfs);
    rootfs = (struct mount*)kmalloc(sizeof(struct mount));
    tmpfs.setup_mount(&tmpfs, rootfs);
}

int register_filesystem(struct filesystem* fs) {
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.
    if (!strcmp(fs->name, "tmpfs")) {
        int err = tmpfs_register();
        if (err == 0) {
            uart_printf("\n[%f] Register tmpfs", get_timestamp());
        }
        return err;
    }
    else if (!strcmp(fs->name, "fat32")) {
        int err = fat32_register();
        if (err == 0) {
            uart_printf("\n[%f] Register fat32", get_timestamp());
        }
        return err;
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

void traversal_recursive(struct dentry* node, const char* path, struct vnode** target_node, char* target_path) {
    // find next /
    int i = 0;
    while (path[i]) {
        if (path[i] == '/') break;
        target_path[i] = path[i];
        i++;
    }
    target_path[i++] = '\0';
    *target_node = node->vnode;
    // edge cases check
    if (!strcmp(target_path, "")) {
        return;
    }
    else if (!strcmp(target_path, ".")) {
        traversal_recursive(node, path + i, target_node, target_path);
        return;
    }
    else if (!strcmp(target_path, "..")) {
        if (node->parent == NULL) { // root directory TODO: mountpoint
            return;
        }
        traversal_recursive(node->parent, path + i, target_node, target_path);
        return;
    }
    // find in node's child
    struct list_head* p;
    list_for_each(p, &node->childs) {
        struct dentry* dent = list_entry(p, struct dentry, list);
        if (!strcmp(dent->name, target_path)) {
            if (dent->mountpoint != NULL) {
                traversal_recursive(dent->mountpoint->root, path + i, target_node, target_path);
            }
            else if (dent->type == DIRECTORY) {
                traversal_recursive(dent, path + i, target_node, target_path);
            }
            break;
        }
    }
}

void traversal(const char* pathname, struct vnode** target_node, char* target_path) {
    if (pathname[0] == '/') {  // absolute path
        struct vnode* rootnode = rootfs->root->vnode;
        traversal_recursive(rootnode->dentry, pathname + 1, target_node, target_path);
    }
    else {  // relative path
        struct vnode* rootnode = current_task->pwd->vnode;
        traversal_recursive(rootnode->dentry, pathname, target_node, target_path);
    }
}

struct file* vfs_open(const char* pathname, int flags) {
    // 1. Find target_dir node and target_path based on pathname
    struct vnode* target_dir;
    char target_path[128];
    traversal(pathname, &target_dir, target_path);
    // 2. Create a new file descriptor for this vnode if found.
    struct vnode* target_file;
    if (target_dir->v_ops->lookup(target_dir, &target_file, target_path) == 0) {
        return create_fd(target_file);
    }
    // 3. Create a new file if O_CREAT is specified in flags.
    else {
        if (flags & O_CREAT) {
            int res = target_dir->v_ops->create(target_dir, &target_file, target_path);
            if (res < 0) return NULL; // error
            target_file->dentry->type = REGULAR_FILE;
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
    if (file->vnode->dentry->type != REGULAR_FILE) {
        uart_printf("Write to non regular file\n");
        return -1;
    }
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
    return file->f_ops->write(file, buf, len);
}

int vfs_read(struct file* file, void* buf, uint64_t len) {
    if (file->vnode->dentry->type != REGULAR_FILE) {
        uart_printf("Read on non regular file\n");
        return -1;
    }
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
    int res = target_dir->v_ops->mkdir(target_dir, &child_dir, child_name);
    if (res < 0) return res; // error
    child_dir->dentry->type = DIRECTORY;
    return 0;
}

int vfs_chdir(const char* pathname) {
    struct vnode* target_dir;
    char path_remain[128];
    traversal(pathname, &target_dir, path_remain);
    if (strcmp(path_remain, "")) { // not found
        return -1;
    }
    else {
        current_task->pwd = target_dir->dentry;
        return 0;
    }
}

// error: -1: not directory, -2: not found
int vfs_mount(const char* device, const char* mountpoint, const char* filesystem) {
    // check mountpoint is valid
    struct vnode* mount_dir;
    char path_remain[128];
    traversal(mountpoint, &mount_dir, path_remain);
    if (!strcmp(path_remain, "")) {  // found
        if (mount_dir->dentry->type != DIRECTORY) {
            return -1;
        }
    }
    else {
        return -2;
    }

    // mount fs on mountpoint
    struct mount* mt = (struct mount*)kmalloc(sizeof(struct mount));
    mt->dev_name = (char*)kmalloc(sizeof(char) * strlen(device));
    strcpy(mt->dev_name, device);
    if (!strcmp(filesystem, "tmpfs")) {
        tmpfs.setup_mount(&tmpfs, mt);
        mount_dir->dentry->mountpoint = mt;
        mt->root->mount_parent = mount_dir->dentry;
    }

    return 0;
}

// error: -1: not directory, -2: not a mount point, -3: not found
int vfs_umount(const char* mountpoint) {
    // check mountpoint is valid
    struct vnode* mount_dir;
    char path_remain[128];
    traversal(mountpoint, &mount_dir, path_remain);
    if (!strcmp(path_remain, "")) {  // found
        if (mount_dir->dentry->type != DIRECTORY) {
            return -1;
        }
        if (!mount_dir->dentry->mount_parent) {
            return -2;
        }
    }
    else {
        return -3;
    }

    // umount
    struct list_head *p;
    list_for_each(p, &mount_dir->dentry->childs) {
        struct dentry *dentry = list_entry(p, struct dentry, list);
        list_del(p);
        kfree(dentry);
    }
    mount_dir->dentry->mount_parent->mountpoint = NULL;

    return 0;
}