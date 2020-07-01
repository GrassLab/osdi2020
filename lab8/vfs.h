#ifndef _VFS_H
#define _VFS_H

#define FILE_TABLE_SIZE 16
#define SUCCESS 0
#define FAIL -1

typedef struct vnode {
    struct mount* mount;
    struct vnode_operations* v_ops;
    struct file_operations* f_ops;
    void* internal;
} vnode_t;

typedef struct file {
    vnode_t* vnode;
    unsigned long f_pos;
    unsigned long file_size;
    struct file_operations* f_ops;
    int flags;
} file_t;

typedef struct mount {
    struct vnode* root;
    struct filesystem* fs;
} mount_t;

typedef struct filesystem {
    const char* name;
    int (*setup_mount)(struct filesystem* fs, mount_t** mount);
} filesystem_t;

typedef struct file_operations {
    int (*write)(file_t* file, const void* buf, unsigned long len);
    int (*read)(file_t* file, void* buf, unsigned long len);
} file_operations_t;

typedef struct vnode_operations {
    int (*lookup)(vnode_t* dir_node, vnode_t** target, const char* component_name);
    int (*create)(vnode_t* dir_node, vnode_t** target, const char* component_name);
} vnode_operations_t;

enum mode { O_OPEN,
    O_CREAT };
extern mount_t* rootfs;

int register_filesystem(filesystem_t* fs);
file_t* vfs_open(const char* pathname, int flags);
int vfs_close(file_t* file);
int vfs_write(file_t* file, const void* buf, unsigned long len);
int vfs_read(file_t* file, void* buf, unsigned long len);

#endif