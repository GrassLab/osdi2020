#ifndef _VFS_H
#define _VFS_H

typedef struct vnode {
    mount_t* mount;
    vnode_operation_t* v_ops;
    file_operations_t* f_ops;
    void* internal;
} vnode_t;

typedef struct file {
    vnode_t* vnode;
    unsigned long f_ops;
    file_operations_t* f_ops;
    int flags;
} file_t;

typedef struct mount {
    vnode_t* root;
    filesystem_t* fs;
} mount_t;

typedef struct filesystem {
    const char* name;
    int (*setup_mount)(filesystem_t* fs, mount_t* mount);
} filesystem_t;

typedef struct file_operations {
    int (*write)(file_t* file, const void* buf, unsigned long len);
    int (*read)(file_t* file, void* buf, unsigned long len);
} file_operations_t;

typedef struct vnode_operations {
    int (*lookup)(vnode_t* dir_node, vnode_t** target, const char* component_name);
    int (*create)(vnode_t* dir_node, vnode_t** target, const char* component_name);
} vnode_operations_t;

mount_t* rootfs;

int register_filesystem(filesystem_t* fs);
file_t* vfs_open(const char* pathname, int flags);
int vfs_close(file_t* file);
int vfs_write(file_t* file, const void* buf, unsigned long len);
int vfs_read(file_t* file, void* buf, unsigned long len);

#endif