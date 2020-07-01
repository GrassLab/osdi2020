#ifndef VFS_H
#define VFS_H

#define REGULAR_FILE 0 
#define DIRECTORY 1
#define ROOT_DIR 2

#define O_CREAT 1

typedef struct vnode {
    struct mount *mount;
    struct vnode_operations *v_ops;
    struct file_operations *f_ops;
    void *internal;
} VNode;

typedef struct file {
    VNode *vnode;
    struct file_operations *f_ops;
    int f_pos; // The next read/write position of this file descriptor
    int flags;
} File;

typedef struct mount {
    VNode *root;
    struct filesystem *fs;
} Mount;

typedef struct filesystem {
    const char *name;
    int (*setup_mount)(struct filesystem *fs, Mount *mount);
} FileSystem;

typedef struct file_operations {
    int (*write) (File *file, const void *buf, int len);
    int (*read) (File *file, void *buf, int len);
} FileOperations;

typedef struct vnode_operations {
    int (*ls)(VNode *node);
    int (*lookup)(VNode *dir_node, VNode **target, const char *component_name);
    int (*create)(VNode *dir_node, VNode **target, const char *component_name);
    int (*mkdir)(VNode *dir_node, VNode **target, const char *component_name);
} VNodeOperations;

Mount *rootfs;

int register_filesystem(FileSystem *fs);
File* vfs_open(const char *pathname, int flags);
int vfs_close(File *file);
int vfs_write(File *file, const void *buf, int len);
int vfs_read(File *file, void *buf, int len);
int vfs_ls(VNode *vnode);
int vfs_mkdir(VNode *dir_node, char *dir_name);

#endif 