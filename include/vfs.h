#ifndef VFS_H
#define VFS_H

#include "type.h"

#define MAX_CHILD_DIR 10

typedef enum file_op_flag
{
    O_CREAT = 0b1,

} file_op_flag_t;

typedef enum node_attr_flag
{
    DIRECTORY,
    FILE,
} node_attr_flag_t;

typedef struct vnode
{
    struct mount * mount;
    struct vnode_operations * v_ops;
    struct file_operations * f_ops;

} vnode_t;

typedef struct file
{
    struct dentry * dentry;
    size_t f_pos;  // The next read/write position of this file descriptor
    // struct file_operations * f_ops;
    // int flags;

} file_t;

typedef struct dentry
{
    char * name;
    node_attr_flag_t flag;
    vnode_t * vnode;
    int child_amount;
    struct dentry * parent_dentry;
    struct dentry * child_dentry[MAX_CHILD_DIR];

    void * internal;

} dentry_t;

typedef struct mount
{
    struct vnode * root;
    struct filesystem * fs;
    dentry_t * root_dir;

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
    int ( *lookup ) ( struct dentry * dir_node, struct dentry ** target, const char * component_name );
    int ( *create ) ( struct dentry * dir_node, struct dentry ** target, const char * component_name );

} vnode_op_t;

int register_filesystem ( struct filesystem * fs );
file_t * vfs_open ( const char * pathname, file_op_flag_t flags );
int vfs_close ( struct file * file );
int vfs_write ( struct file * file, const void * buf, size_t len );
int vfs_read ( struct file * file, void * buf, size_t len );

#endif