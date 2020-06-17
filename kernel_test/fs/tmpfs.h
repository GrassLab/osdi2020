#define TMPFS_CONTENT_SIZE 0x1000
#define FILE        0x0
#define DIRECTORY   0x1


struct tmpfs_internal {
    int type;
    char content[TMPFS_CONTENT_SIZE];
    int content_size;
};

void setup_vnode(struct vnode** vnode);
void setup_dentry(struct dentry** dentry);
int setup_tmpfs_filesystem();
int setup_tmpfs_mount(struct filesystem* fs, struct mount* mount);
int tmpfs_write(struct file* file, const void* buf, int len);
int tmpfs_read(struct file* file, void* buf, int len);
int tmpfs_lookup(struct dentry* dentry, struct vnode** target, const char* component_name);
int tmpfs_create(struct dentry* parent, struct vnode** target, const char* component_name);