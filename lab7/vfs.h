#include <stddef.h>
#ifndef __VFS_H__
#define __VFS_H__

struct vfs_vnode_struct
{
  struct vfs_mount_struct * mount;
  struct vfs_vnode_operations_struct * v_ops;
  struct vfs_file_operations_struct * f_ops;
  void * internal;
};

struct vfs_file_struct
{
  struct vfs_vnode_struct * vnode;
  size_t f_pos; /* The next read/write position of this file descriptor */
  struct vfs_file_operations_struct * f_ops;
  int flags;
};

struct vfs_mount_struct
{
  struct vfs_vnode_struct * root;
  struct vfs_filesystem_struct * fs;
};

struct vfs_filesystem_struct
{
  const char * name;
  int (*setup_mount)(struct vfs_filesystem_struct * fs, struct vfs_mount_struct * mount);
};

struct vfs_file_operations_struct
{
  int (*write) (struct vfs_file_struct * file, const void * buf, size_t len);
  int (*read) (struct vfs_file_struct * file, void * buf, size_t len);
};

struct vfs_vnode_operations_struct
{
  int (*lookup)(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name);
  int (*create)(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name);
};


int vfs_regist_ffs(struct vfs_filesystem_struct * fs);
struct vfs_file_struct * vfs_open(const char * pathname, int flags);
int vfs_close(struct vfs_file_struct * file);
int vfs_write(struct vfs_file_struct * file, const void * buf, size_t len);
int vfs_read(struct vfs_file_struct * file, void * buf, size_t len);

#endif

