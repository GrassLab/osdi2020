#ifndef __VFS__H__
#define __VFS__H__

#define O_CREAT (1)

struct vnode {
    char mem[1024];
    int cluster;
    int size;
    int fat_dir_id;
    struct file_descriptor_operations* fd_ops;
};

struct entry_list {
    int used;
    struct dentry *child;
};

enum filetype{
    directory, file
};

struct dentry {
    char name[32];
    enum filetype type;
    struct vnode *vnode;
    struct dentry *parent;
    int count;
    struct entry_list d_list[16];
    struct dentry_operations *d_ops;
};

struct file_descriptor {
    struct vnode* vnode;
    int f_pos; // The next read/write position of this file descriptor
    struct file_descriptor_operations* fd_ops;
    int flags;
};

struct mount {
    struct dentry* root;
    struct filesystem* fs;
};

struct filesystem {
    const char* name;
    struct dentry *root;
    int (*setup_mount)(struct filesystem* fs, struct mount* mount);
};

// file descriptor to read/write vnode
struct file_descriptor_operations {
    int (*write) (struct file_descriptor *fd, char *buf, int len);
    int (*read) (struct file_descriptor *fd, char *buf, int len);
};

// dentry function
struct dentry_operations {
    int (*lookup)(struct dentry *dir_node, struct dentry **target, const char *component_name);
    int (*create)(struct dentry *parent, struct dentry **target, const char *component_name, enum filetype type);
};

extern struct mount rootfs;
extern struct dentry dentryObj[64];
extern int dentryObjNum;
extern struct vnode vnodeObj[64];
extern int vnodeObjNum;
extern struct file_descriptor fdObj[64];
extern int fdObjNum;

int register_filesystem(struct filesystem* fs);
struct file_descriptor* vfs_open(const char* pathname, int flags);
void vfs_close(struct file_descriptor *fd);
int vfs_read(struct file_descriptor *fd, char *buf, int len);
void vfs_write(struct file_descriptor *fd, char *buf, int len);
void fd_lseek(struct file_descriptor *fd, int pos);

#endif