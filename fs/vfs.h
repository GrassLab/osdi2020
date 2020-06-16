#define O_CREAT 0xdeadbeaf
#define DENTRY_MAX_CHILD 0x10
#define NAME_LENGTH 0x100

#define LOG_MODE 1
#define log(msg) if(LOG_MODE==1){printf(msg);}

// typedef int size_t;

struct fs_manager {
    struct filesystem* recognize_fs[0x10];
    int num;
};

struct vnode {
    struct mount* mount;
    struct vnode_operations* v_ops;
    struct file_operations* f_ops;
    void* internal; // A VFS should call the underlying file system specific method to construct the root directory’s vnode.
};

struct dentry {
    struct vnode* vnode;
    struct dentry* parent;
    struct dentry* child[DENTRY_MAX_CHILD];
    int child_num;
    char name[NAME_LENGTH];
};

struct file {
    struct vnode* vnode;
    int f_pos; // The next read/write position of this file descriptor
    struct file_operations* f_ops;
    int flags;
};

struct mount { 
    struct vnode* root;
    struct dentry* dentry;
    struct filesystem* fs;
};

struct filesystem {
    char name[NAME_LENGTH];
    int (*setup_mount)(struct filesystem* fs, struct mount* mount);
};

struct file_operations {
    int (*write) (struct file* file, const void* buf, int len);
    int (*read) (struct file* file, void* buf, int len);
};

struct vnode_operations {
    int (*lookup)(struct dentry* dentry, struct vnode** target, const char* component_name);
    int (*create)(struct dentry* dentry, struct vnode** target, const char* component_name);
};

int register_filesystem(struct filesystem* fs) ;
struct file* vfs_open(const char* pathname, int flags);
int vfs_close(struct file* file);
int vfs_write(struct file* file, const void* buf, int len);
int vfs_read(struct file* file, void* buf, int len);
