#define O_CREAT 0xdeadbeaf
#define DENTRY_MAX 0x20
#define NAME_MAX 0x20

struct vnode {
	// struct vnode *vnode;
	struct mount *mount;
	struct vnode_operations *v_ops;
	struct file_operations *f_ops;
	void *internal;
};

struct dentry {
	struct vnode *vnode;
	struct dentry *parent;
	struct dentry *child[DENTRY_MAX];
	int count;
	char name[NAME_MAX];
};

struct file {
	struct vnode *vnode;
	int f_pos;
	struct file_operations *f_ops;
	int flags;
};

struct mount {
	struct vnode *root;
	struct dentry *dentry;
	struct filesystem *fs;
};

struct filesystem {
	char name[NAME_MAX];
	int (*setup_mount)(struct filesystem *fs, struct mount *mount);
};

struct file_operations {
	int (*write)(struct file *file, const void *buf, int len);
	int (*read)(struct file *file, void *buf, int len);
};

struct vnode_operations {
	int (*lookup)(struct dentry *folder, struct vnode **target,
		      const char *component_name);
	int (*create)(struct dentry *folder, struct vnode **target,
		      const char *component_name);
};

int register_filesystem(struct filesystem *fs);
struct file *vfs_open(const char *pathname, int flags);
int vfs_close(struct file *file);
int vfs_write(struct file *file, const void *buf, int len);
int vfs_read(struct file *file, void *buf, int len);
