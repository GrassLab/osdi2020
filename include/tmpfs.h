#ifndef TMPFS_H
#define TMPFS_H

#include "shell.h"

#define FIXED 0
#define DIR_MAX 4
#define TMP_BUF_SIZE 512


typedef enum {
  FILE,
  DICTIONARY,
  NONE
} FILE_TYPE;

struct tmpfs_buf{
  int flag;
  unsigned long size;
  char buffer[TMP_BUF_SIZE];
};

struct dentry {
  char name[10];
  FILE_TYPE type;
  struct vnode* vnode;
  struct dentry* list[DIR_MAX];
  struct tmpfs_buf* buf;
};

extern struct file_operations* tmpfs_f_ops;
extern struct vnode_operations* tmpfs_v_ops;

void init_dentry(struct dentry* dentry, struct vnode* vnode, const char* name);
int tmpfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);
int tmpfs_create(struct vnode* dir_node, struct vnode** target, const char* component_name);
int tmpfs_mount(struct filesystem* fs, struct mount* mount);
int tmpfs_write(struct file* file, const void* buf, unsigned long len);
int tmpfs_read(struct file* file, void* buf, unsigned long len);

#endif
