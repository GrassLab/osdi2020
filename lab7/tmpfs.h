#define EOF (-1)

#define TMP_FILE_SIZE 512
struct tmpfs_node
{
    char buffer[TMP_FILE_SIZE];
    int objid;
};

struct vnode_operations *tmpfs_v_ops;
struct file_operations *tmpfs_f_ops;

void set_tmpfs_vnode(struct vnode *vnode);
int setup_mount_tmpfs(struct filesystem *fs, struct mount *mount);
int lookup_tmpfs(struct dentry *dir, struct vnode **target, char *component_name);
int create_tmpfs(struct dentry *dir, struct vnode **target, char *component_name);
int write_tmpfs(struct file *file, void *buf, unsigned long len);
int read_tmpfs(struct file *file, void *buf, unsigned long len);
