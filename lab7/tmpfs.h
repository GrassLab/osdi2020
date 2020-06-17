#define NUM_OF_FILESYS 4
#define DENTRY_MAX_CHILD 10
#define DIR_NAME_LEN 20
#define O_CREAT 2


struct dentry {
  char *name;
  int flag;
  struct vnode* vnode;
  struct vnode *parent;
};

struct vnode {
  struct mount* mount;
  struct vnode_operations* v_ops;
  struct file_operations* f_ops;
  void* internal;
  int num_of_child;
  struct dentry *child[DENTRY_MAX_CHILD];
};

struct file {
  struct vnode* vnode;
  int f_pos; // The next read/write position of this file descriptor
  struct file_operations* f_ops;
  int flags;
};

struct mount {
  struct vnode* root;
  struct filesystem* fs;
};

struct filesystem {
  const char* name;
  int (*setup_mount)(struct filesystem* fs, struct mount* mount);
};

struct file_operations {
  int (*write) (struct file* file, const void* buf, int len);
  int (*read) (struct file* file, void* buf, int len);
};

struct vnode_operations {
  int (*lookup)(struct vnode* dir_node, struct vnode** target, const char* component_name);
  int (*create)(struct vnode* dir_node, struct vnode** target, const char* component_name);
};

void filesystem_init();
int register_filesystem(struct filesystem* fs);
struct file* vfs_open(const char* pathname, int flags);
int vfs_close(struct file* file);
int vfs_write(struct file* file, const void* buf, int len);
int vfs_read(struct file* file, void* buf, int len);
void rootfs_init();