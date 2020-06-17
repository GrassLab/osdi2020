#ifndef KERNEL_VFS_H_
#define KERNEL_VFS_H_

#include "kernel/lib/types.h"

#define MAX_FILESYSTEM_NUM 8
#define MAX_PATHNAME_LEN 512
#define O_CREAT 00000100

enum vnode_type {
  VNODE_TYPE_DIR,
  VNODE_TYPE_FILE
};

struct vnode {
  struct mount* mount;
  struct file_operations* f_ops;
  struct vnode_operations* v_ops;
  enum vnode_type type;
  void* internal;
};

struct file {
  struct vnode* vnode;
  size_t f_pos; // The next read/write position of this file descriptor
  struct file_operations* f_ops;
  int flags;
};

struct mount {
  struct filesystem* fs;
  struct vnode* root;
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
  int (*lookup)(struct vnode* dir_node, struct vnode** target, const char* component_name);
  int (*create)(struct vnode* dir_node, struct vnode** target, const char* component_name);
};

size_t filesystem_num;
struct filesystem *filesystems[MAX_FILESYSTEM_NUM];

struct mount* rootfs;

int register_filesystem(struct filesystem* fs);

struct file* vfs_open(const char* pathname, int flags);
int vfs_close(struct file* file);
int vfs_write(struct file* file, const void* buf, size_t len);
int vfs_read(struct file* file, void* buf, size_t len);

#endif // KERNEL_VFS_H_