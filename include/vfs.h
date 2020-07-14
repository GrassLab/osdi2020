#ifndef _VFS_H
#define _VFS_H

#define O_CREAT 1
#define FIXED 0

extern struct mount* rootfs;
extern struct filesystem tmpfs;

struct vnode {
  struct mount* mount;
  struct vnode_operations* v_ops;
  struct file_operations* f_ops;
  void* internal;
};

struct file {
  struct vnode* vnode;
  unsigned long f_pos; // The next read/write position of this file descriptor
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
  int (*write) (struct file* file, const void* buf, unsigned long len);
  int (*read) (struct file* file, void* buf, unsigned long len);
};

struct vnode_operations {
  int (*lookup)(struct vnode* dir_node, struct vnode** target, const char* component_name);
  int (*create)(struct vnode* dir_node, struct vnode** target, const char* component_name);
};


int register_filesystem(struct filesystem* fs);
struct file* vfs_open(const char* pathname, int flags);
int vfs_close(struct file* file);
int vfs_write(struct file* file, const void* buf, unsigned long len);
int vfs_read(struct file* file, void* buf, unsigned long len);
void vfs_ls(const char* component_name);
void vfs_mkdir(const char* component_name);

#endif
