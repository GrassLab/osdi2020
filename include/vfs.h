#ifndef VFS_H
#define VFS_H

#include "type.h"

typedef struct vnode
{
    struct mount * mount;
    struct vnode_operations * v_ops;
    struct file_operations * f_ops;
    void * internal;
} vnode_t;

typedef struct file
{
    struct vnode * vnode;
    size_t f_pos;  // The next read/write position of this file descriptor
    struct file_operations * f_ops;
    int flags;
} file_t;

typedef struct mount
{
    struct vnode * root;
    struct filesystem * fs;
} mount_t;

typedef struct filesystem
{
    const char * name;
    int ( *setup_mount ) ( struct filesystem * fs, struct mount * mount );
} file_sys_t;

typedef struct file_operations
{
    int ( *write ) ( struct file * file, const void * buf, size_t len );
    int ( *read ) ( struct file * file, void * buf, size_t len );
} file_op_t;

typedef struct vnode_operations
{
    int ( *lookup ) ( struct vnode * dir_node, struct vnode ** target, const char * component_name );
    int ( *create ) ( struct vnode * dir_node, struct vnode ** target, const char * component_name );
} vnode_op_t;

int register_filesystem ( struct filesystem * fs );
struct file * vfs_open ( const char * pathname, int flags );
int vfs_close ( struct file * file );
int vfs_write ( struct file * file, const void * buf, size_t len );
int vfs_read ( struct file * file, void * buf, size_t len );

#endif