#pragma once

#include "unistd.h"

#define O_CREAT 1

struct vnode {
  struct mount *mount;
  struct vnode_operations *v_ops;
  struct file_operations *f_ops;
  int count;
  size_t size;
  int is_dir;
  struct vnode *parent;
  struct vnode *next; /* same level node will placed here */
  struct vnode *sub_dir;
  /* struct file *file; */
  const char *basename;
  void *internal;
};

struct file {
  int f_id;
  const char *name;
  struct vnode *vnode;
  size_t f_pos; // The next read/write position of this file descriptor
  struct file_operations *f_ops;
  int flags;
};

struct mount {
  struct vnode *root;
  struct filesystem *fs;
};

struct filesystem {
  const char *name;
  int (*setup_mount)(struct filesystem *fs, struct mount *mount);
};

struct file_operations {
  int (*write)(struct file *file, const void *buf, size_t len);
  int (*read)(struct file *file, void *buf, size_t len);
};

struct vnode_operations {
  int (*lookup)(struct vnode *dir_node, struct vnode **target,
                const char *component_name);
  int (*create)(struct vnode *dir_node, struct vnode **target,
                const char *component_name);
  struct file *(*create_file)(const char *pathname, struct vnode *target);
};

extern struct mount *rootfs;
extern struct vnode *cur_root;

struct file *vfs_open(const char *pathname, int flags);
int vfs_close(struct file *file);
int vfs_write(struct file *file, const void *buf, size_t len);
int vfs_read(struct file *file, void *buf, size_t len);
int vfs_mkdir(const char *pathname);
int vfs_chdir(const char *pathname);
int vfs_mount(const char *device, const char *mountpoint,
              const char *filesystem);
int vfs_umount(const char *mountpoint);

/* path name solver */
struct path {
  const char *name;
  struct path *next;
};


struct path *path_name_solver(const char *pathname);
