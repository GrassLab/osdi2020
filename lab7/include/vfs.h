#ifndef _VFS_H_
#define _VFS_H_

#include "config.h"

#define MAX_CHILD_NUMBER 8
#define DNAME_LEN 16
enum
{
    O_CREAT = 1
};

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

typedef struct dentry_t 
{
    char dname[DNAME_LEN];
    struct vnode_t* vnode;
    int child_count;
    struct dentry_t* child_dentry;
} dentry_t; 

typedef struct mount_t
{
    struct vnode_t *root;
    struct filesystem_t *fs;
    struct dentry_t* dentry;
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
    int (*lookup)(dentry_t *dir_node, vnode_t **target, const char *component_name);
    int (*create)(dentry_t *dir_node, vnode_t **target, const char *component_name);
} vnode_operations_t;

int register_filesystem(filesystem_t *fs);
file_t *vfs_open(const char *pathname, int flags);
int vfs_close(file_t *file);
int vfs_write(file_t *file, const void *buf, size_t len);
int vfs_read(file_t *file, void *buf, size_t len);
void set_dentry(dentry_t *dentry, vnode_t *vnode, const char* dir_name);
void rootfs_init();

mount_t *rootfs;

#endif 