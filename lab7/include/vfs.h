#ifndef _VFS_H_
#define _VFS_H_

#include "config.h"

typedef struct vnode_t
{
    struct mount_t *mount;
    struct vnode_operations_t *v_ops;
    struct file_operations_t *f_ops;
    void *internal;
} vnode_t;

typedef struct file_t
{
    struct vnode_t *vnode;
    size_t f_pos; // The next read/write position of this file descriptor
    struct file_operations_t *f_ops;
    int flags;
} file_t;

typedef struct mount_t
{
    struct vnode_t *root;
    struct filesystem_t *fs;
} mount_t;

typedef struct filesystem_t
{
    const char *name;
    int (*setup_mount)(struct filesystem_t *fs, mount_t *mount);
} filesystem_t;

typedef struct file_operations_t
{
    int (*write)(file_t *file, const void *buf, size_t len);
    int (*read)(file_t *file, void *buf, size_t len);
} file_operations_t;

typedef struct vnode_operations_t
{
    int (*lookup)(vnode_t *dir_node, vnode_t **target, const char *component_name);
    int (*create)(vnode_t *dir_node, vnode_t **target, const char *component_name);
} vnode_operations_t;

int register_filesystem(filesystem_t *fs);
file_t *vfs_open(const char *pathname, int flags);
int vfs_close(file_t *file);
int vfs_write(file_t *file, const void *buf, size_t len);
int vfs_read(file_t *file, void *buf, size_t len);

enum
{
    O_CREAT = 1
};

mount_t *rootfs;
mount_t rootfs_location;


#endif 