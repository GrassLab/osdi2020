#ifndef _VFS_H
#define _VFS_H


#define O_CREAT 1

#define DNAME_LEN 32
#define MAX_CHILD 8

struct dentry {
    // Pointer to the inode this entry refers to.
    char dname[DNAME_LEN];
    struct vnode* vnode;
    int child_count;
    struct dentry *child_dentry;
}dentry;   


struct vnode {
    struct mount* mount;
    struct vnode_operations* v_ops;
    struct file_operations* f_ops;
    void* internal;
};

struct file {
    struct vnode* vnode;
    unsigned f_pos; // The next read/write position of this file descriptor
    struct file_operations* f_ops;
    int flags;
};

struct mount {
    struct vnode* root;
    struct dentry* dentry;
    struct filesystem* fs;
};

struct filesystem {
    const char* name;
    int (*setup_mount)(struct filesystem* fs, struct mount* mount);
};

struct file_operations {
    int (*write) (struct file* file, const void* buf, unsigned len);
    int (*read) (struct file* file, void* buf, unsigned len);
};

struct vnode_operations {
    int (*lookup)(struct dentry* dir, struct vnode** target, const char* component_name);
    int (*create)(struct dentry* dir, struct vnode** target, const char* component_name);
};

struct mount* rootfs;

void set_dentry(struct dentry *dentry,struct vnode* vnode,const char* str);
void rootfs_init();
int register_filesystem(struct filesystem* fs);
struct file* vfs_open(const char* pathname, int flags);
int vfs_close(struct file* file);
int vfs_write(struct file* file, const void* buf, unsigned len);
int vfs_read(struct file* file, void* buf, unsigned len);

#endif