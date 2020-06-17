#ifndef _VFS_H
#define _VFS_H

typedef unsigned long long size_t;

#define O_CREAT 1

#define DNAME_LEN 32
#define MAX_CHILD 8

struct dentry {
    /* Pointer to the inode this entry refers to. */
    int flag;
    char dname[DNAME_LEN];
    struct vnode* vnode;
    int child_count;
    struct dentry *parent_dentry;
    struct dentry *child_dentry[MAX_CHILD];
};   

struct vnode {
  struct mount* mount;
  struct vnode_operations* v_ops;
  struct file_operations* f_ops;
  void* internal;
};

struct file {
  struct vnode* vnode;
  size_t f_pos; // The next read/write position of this file descriptor
  struct file_operations* f_ops;
  int flags;
};

struct mount {
  struct vnode* root;
  struct filesystem* fs;
  struct dentry* dentry;
};

struct filesystem {
  const char* name;
  int (*setup_mount)(struct filesystem* fs, struct mount* mount);
};

struct file_operations {
  int (*read) (struct file* file, void* buf, size_t len);
  int (*write) (struct file* file, const void* buf, size_t len);
};

struct vnode_operations {
  int (*lookup)(struct dentry* dir_node, struct vnode** target, const char* component_name);
  int (*create)(struct dentry* dir_node, struct vnode** target, const char* component_name);
};

extern struct mount* rootfs;
extern struct dentry* current_dent;

#define MOUNT_TABLE_SIZE    8
extern char mt_table_map[MOUNT_TABLE_SIZE];
extern struct mount* mount_fs_table[MOUNT_TABLE_SIZE];

void rootfs_init();
void set_dentry(struct dentry *dentry, struct vnode *node, const char *str);
int register_filesystem(struct filesystem* fs);
struct file* vfs_open(const char* pathname, int flags);
int vfs_close(struct file* file);
int vfs_write(struct file* file, const void* buf, size_t len);
int vfs_read(struct file* file, void* buf, size_t len);

#endif
