#include "tmpfs.h"

struct vnode_operations* tmpfs_v_ops;
struct file_operations* tmpfs_f_ops;

int tmpfs_register() {
    tmpfs_v_ops->create = tmpfs_create;
    tmpfs_v_ops->lookup = tmpfs_lookup;
    tmpfs_f_ops->read = tmpfs_read;
    tmpfs_f_ops->write = tmpfs_write;
    return 0;
}

int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount) {
}

// vnode operations
struct dentry* tmpfs_lookup(struct vnode* dir, struct dentry* dentry) {
}

int tmpfs_create(struct vnode* dir, struct dentry* dentry) {
}

// file operations
int tmpfs_read(struct file* file, void* buf, uint64_t len) {
}

int tmpfs_write(struct file* file, const void* buf, uint64_t len) {
}
