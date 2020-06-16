#include "tmpfs.h"

#include "mm.h"
#include "my_string.h"

struct vnode_operations* tmpfs_v_ops;
struct file_operations* tmpfs_f_ops;

struct dentry* tmpfs_create_root() {
    struct dentry* dentry = (struct dentry*)kmalloc(sizeof(struct dentry));
    dentry->name = (char*)kmalloc(sizeof(char) * 2);
    strcpy(dentry->name, "/");
    dentry->parent = NULL;
    return dentry;
}

struct vnode* tmpfs_create_vnode(struct dentry* dentry) {
    struct vnode* vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
    vnode->dentry = dentry;
    vnode->f_ops = tmpfs_f_ops;
    vnode->v_ops = tmpfs_v_ops;
    return vnode;
}

int tmpfs_register() {
    tmpfs_v_ops = (struct vnode_operations*)kmalloc(sizeof(struct vnode_operations));
    tmpfs_v_ops->create = tmpfs_create;
    tmpfs_v_ops->lookup = tmpfs_lookup;
    tmpfs_f_ops = (struct file_operations*)kmalloc(sizeof(struct file_operations));
    tmpfs_f_ops->read = tmpfs_read;
    tmpfs_f_ops->write = tmpfs_write;
    return 0;
}

int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount) {
    // create root directory entry
    struct dentry* root_dentry = tmpfs_create_root();

    // create vnode
    root_dentry->vnode = tmpfs_create_vnode(root_dentry);

    // mount on mount point
    mount->fs = fs;
    mount->root = root_dentry;
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
