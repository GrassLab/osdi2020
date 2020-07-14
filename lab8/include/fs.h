#ifndef __VFS_H__
#define __VFS_H__

#include "type.h"

struct vnode {
  struct mount *mount;
  struct vnode_operations *v_ops;
  struct file_operations *f_ops;
  struct directory_operations *d_ops;
  void *internal;
};

struct mount {
  struct vnode *mp;
  struct vnode *root;
  struct filesystem *fs;
};

struct filesystem {
  const char *name;
  int (*setup_mount)(struct filesystem *fs, struct mount *mount);
  struct filesystem *nextfs;
  struct mount *mnt;
};

typedef struct file {
  struct vnode *vnode;
  size_t f_pos;
  // The next read/write position of this file descriptor
  struct file_operations *f_ops;
  int flags;
} FILE;

enum dirent_type{
  dirent_none,
  dirent_file,
  dirent_dir,
};

typedef struct dirent {
  const char *name;
  enum dirent_type type;
} dirent;

typedef struct dir{
  char *path;
  struct vnode *root;
  struct vnode *child;
  dirent entry;  
  struct directory_operations *dops;
} DIR;

struct file_operations {
  int (*write) (struct file *file, const void *buf, size_t len);
  int (*read) (struct file *file, void *buf, size_t len);
};

struct directory_operations{
  DIR *(*opendir) (struct vnode *node, DIR *directory, const char *pathname);
  dirent *(*readdir) (DIR *directory);
  int (*mkdir) (struct vnode *node, const char *pathname);
  int (*chdir) (struct vnode *node, const char *pathname);
};

struct vnode_operations {
  int (*lookup)(struct vnode *node, struct vnode **target, const char *component_name);
  int (*create)(struct vnode *node, struct vnode **target, const char *component_name);
  enum dirent_type (*typeof)(struct vnode *node);
};

extern struct mount *rootfs;
int register_filesystem(struct filesystem *fs);
struct file *vfs_open(const char *pathname, int flags);
DIR *vfs_opendir(const char *pathname);
DIR *vfs_opendir_by_node(
    struct vnode *node,
    const char *pathname);
dirent *vfs_readdir(DIR *dir);
void vfs_closedir(DIR *dir);
int vfs_mkdir(const char *path);
int vfs_chdir(const char *path);
int vfs_close(struct file *file);
int vfs_write(struct file *file, const void *buf, size_t len);
int vfs_read(struct file *file, void *buf, size_t len);
void vfs_setup();
void vfs_show();
int vfs_mount(
    const char *dev, const char *mp, const char *fs);
int vfs_umount(const char *mp);

struct mount *newMnt(struct vnode *mp, struct vnode *root);

struct vnode *newVnode(
    struct mount *mount,
    struct vnode_operations *vops,
    struct file_operations *fops, 
    struct directory_operations *dops, 
    void *internal);

void list_dir(DIR *dir, int lv);

#define O_CREAT 0b1
#define EOF     4

void fs_init();
int subpath_of(const char *sub, const char *full, int (*cmp)(char a, char b));
int exist_slash(const char *path);
struct vnode *move_mount_root(struct vnode *node);
#endif
