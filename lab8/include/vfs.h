#ifndef _VFS_H_
#define _VFS_H_
#include "tools.h"

enum
{
    O_OPEN,
    O_CREAT
};
typedef struct vnode_t
{
    struct mount_t *mount;
    struct vnode_operations_t *v_ops;
    struct file_operations_t *f_ops;
    void* internal;
} vnode;

typedef struct file_t
{
    struct vnode_t *vnode;
    size_t f_pos; // The next read/write position of this file descriptor
    struct file_operations_t *f_ops;
    int flags;
} file;

typedef struct mount_t
{
    struct vnode_t *root;
    struct filesystem_t *fs;
} mount;

typedef struct filesystem_t
{
    const char *name;
    int (*setup_mount)(struct filesystem_t *fs, mount **mount);
} filesystem;

typedef struct file_operations_t
{
    int (*write)(struct file_t *file, const void *buf, size_t len);
    int (*read)(struct file_t *file, void *buf, size_t len);
} file_operations;

typedef struct vnode_operations_t
{
    int (*lookup)(struct vnode_t *dir_node, struct vnode_t **target, const char *component_name);
    int (*create)(struct vnode_t *dir_node, struct vnode_t **target, const char *component_name);
} vnode_operations;


int register_filesystem(filesystem *fs);
file *vfs_open(const char *pathname, int flags);
int vfs_close(file *file);
int vfs_write(file *file, const void *buf, size_t len);
int vfs_read(file *file, void *buf, size_t len);


#endif