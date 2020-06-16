#ifndef __VFS__
#define __VFS__

#include "typedef.h"

struct mount {
    struct dentry* root;  // root directory
    struct filesystem* fs;
};

struct filesystem {
    const char* name;
    int (*setup_mount)(struct filesystem* fs, struct mount* mount);
};

struct vnode {
    struct vnode_operations* v_ops;
    struct file_operations* f_ops;
    struct dentry* dentry;  // TODO: list of dentry (link)
    void* internal;         // internal representation of a vnode
};

struct dentry {
    char* name;
    struct dentry* parent;
    struct vnode* vnode;
};

struct file {
    struct vnode* vnode;
    uint64_t f_pos;  // The next read/write position of this file descriptor
    struct file_operations* f_ops;
    int flags;
};

struct file_operations {
    int (*write)(struct file* file, const void* buf, uint64_t len);
    int (*read)(struct file* file, void* buf, uint64_t len);
};

struct vnode_operations {
    struct dentry* (*lookup)(struct vnode* dir, struct dentry* dentry);
    int (*create)(struct vnode* dir, struct dentry* dentry);
};

extern struct mount* rootfs;

void rootfs_init();
int register_filesystem(struct filesystem* fs);
struct file* vfs_open(const char* pathname, int flags);
int vfs_close(struct file* file);
int vfs_write(struct file* file, const void* buf, uint64_t len);
int vfs_read(struct file* file, void* buf, uint64_t len);

#endif // __VFS__