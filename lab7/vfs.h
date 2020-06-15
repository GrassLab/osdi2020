#define O_CREAT 1
#define DNAME_LEN 32
#define MAX_CHILD 8

struct dentry
{
    /* Pointer to the inode this entry refers to. */
    char dname[DNAME_LEN];
    struct vnode *vnode;
    int child_count;
    struct dentry *child_dentry;
    int objid;
} dentry;

struct vnode
{
    struct mount *mount;
    struct vnode_operations *v_ops;
    struct file_operations *f_ops;
    void *internal;
    int objid;
};

struct file
{
    struct vnode *vnode;
    unsigned long f_pos; // The next read/write position of this file descriptor
    struct file_operations *f_ops;
    int flags;
    int objid;
};

struct mount
{
    struct vnode *root;
    struct filesystem *fs;
    struct dentry *dentry;
    int objid;
};

struct filesystem
{
    char *name;
    int objid;
};

struct file_operations
{
    int (*write)(struct file *file, const void *buf, unsigned long len);
    int (*read)(struct file *file, void *buf, unsigned long len);
};

struct vnode_operations
{
    int (*lookup)(struct dentry *dir_node, struct vnode **target, const char *component_name);
    int (*create)(struct dentry *dir_node, struct vnode **target, const char *component_name);
};

struct mount *rootfs;
int register_filesystem(struct filesystem *fs);
struct file *vfs_open(const char *pathname, int flags);
int vfs_close(struct file *file);
int vfs_write(struct file *file, const void *buf, unsigned long len);
int vfs_read(struct file *file, void *buf, unsigned long len);
void set_dentry(struct dentry *dentry, struct vnode *vnode,
                const char *str);
void rootfs_init();
