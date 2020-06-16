#include "tmpfs.h"

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
