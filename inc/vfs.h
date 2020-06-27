#ifndef _VFS_H
#define _VFS_H


#define VNODE_TYPE_REG 0
#define VNODE_TYPE_DIR 1
#define DNAME_LEN 24
#define NR_CHILD 16 
#define REGBUFF_SIZE 512
// #define O_CREAT 1
// #define MAX_CHILD 8

struct dentry {
    char name[DNAME_LEN];
    struct vnode *vnode;
};

// struct dcache {
//     struct dentry[NR_CHILD];
//     // maybe link to an other cache
// };

// struct fcache {
//     char buf[REGBUFF_SIZE];
//     // maybe link to another buf
// };

struct vnode_cache{
    union{
        struct dentry dentries[NR_CHILD];
        char regbuf[REGBUFF_SIZE];
    };
};

struct vnode {
    int type;
    struct mount *mount;// fs mount on this vnode
    struct vnode_operations *v_ops;
    struct file_operations *f_ops;
    struct vnode_cache* cache;
};

struct file {
    struct vnode *vnode;
    // unsigned f_pos; // The next read/write position of this file descriptor
    // struct file_operations *f_ops;
    // int flags;
};

struct mount {
    struct vnode *root;
    struct filesystem *fs;
};

struct filesystem {
    const char *name;
    int (*setup_mount)(struct filesystem *fs, struct mount *mount);
    struct filesystem *next;
};

struct file_operations {
    // int (*read) (struct file *file, void *buf, unsigned len);
    // int (*write) (struct file *file, const void *buf, unsigned len);
};

struct vnode_operations {
    int (*lookup)(struct vnode *vnode, struct vnode **target, const char *component_name);
    int (*create)(struct vnode *vnode, struct vnode **target, const char *component_name);
};

struct filesystem_manager {
    int nr_fs;
    struct filesystem *list;
};

void rootfs_init();
void register_filesystem(struct filesystem *fs);
int vfs_lookup(struct vnode *vnode, struct vnode **target, const char *component_name);
struct file *vfs_open(const char *pathname, int flags);
// int vfs_close(struct file *file);
// int vfs_read(struct file *file, void *buf, unsigned len);
// int vfs_write(struct file *file, const void *buf, unsigned len);

#endif