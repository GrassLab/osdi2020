#ifndef __FS_VFS_H
#define __FS_VFS_H

#include <stddef.h>

#define O_CREAT 1

struct vnode {
    struct mount* mount;
    struct vnode_operations* v_ops;
    struct file_operations* f_ops;
    void* internal;
};

struct file {
    struct vnode* vnode;
    size_t f_pos; // The next read/write position of this file descriptor
    struct file_operations* f_ops;
    int flags;
};

struct filesystem {
    const char* name;
    int (*setup_mount)(struct filesystem* fs, struct mount* mount);
    void (*unset_mount)(struct mount *mount);
};

struct mount {
    struct vnode* root;
    struct filesystem* fs;
};

struct file_operations {
    int (*write) (struct file* file, const void* buf, size_t len);
    int (*read) (struct file* file, void* buf, size_t len);
};

struct vnode_operations {
    int (*lookup)(struct vnode* dir_node, struct vnode** target, const char* component_name);
    int (*create)(struct vnode* dir_node, struct vnode** target, const char* component_name);
};

int register_filesystem(struct filesystem* fs);
void unregister_filesystem(void);
struct filesystem *get_filesystem(const char *name);

struct file* vfs_open(const char* pathname, int flags);
int vfs_close(struct file* file);
int vfs_write(struct file* file, const void* buf, size_t len);
int vfs_read(struct file* file, void* buf, size_t len);

extern struct mount* rootfs;

#endif
