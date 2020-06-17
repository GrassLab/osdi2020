#ifndef VFS_H
#define VFS_H
#include <stddef.h>
#define O_CREAT 1
struct vnode {
  struct mount* mount;
  struct vnode_operations* v_ops;
  struct file_operations* f_ops;
  void* internal;
};

struct file {
  int tmp;
  struct vnode* vnode;
  size_t f_pos; // The next read/write position of this file descriptor
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
  int (*write) (struct file* file, const void* buf, size_t len);
  int (*read) (struct file* file, void* buf, size_t len);
  int (*list)(struct file* file);
};

struct vnode_operations {
  int (*lookup)(struct vnode* dir_node, struct vnode** target, const char* component_name);
  int (*create)(struct vnode* dir_node, struct vnode** target, const char* component_name);
  int (*chdir)(struct vnode* dir_node, struct vnode** target, const char* component_name);
};

extern struct mount* rootfs;
extern struct vnode* currentdir;
extern struct filesystem tmpfs;

void init_rootfs();
int register_filesystem(struct filesystem* fs);
struct file* vfs_open(const char* pathname, int flags);
int vfs_close(struct file* file);
int vfs_write(struct file* file, const void* buf, size_t len);
int vfs_read(struct file* file, void* buf, size_t len);
int vfs_list(struct file* file);
int mkdir(const char* pathname);
int chdir(const char* pathname);
#endif
