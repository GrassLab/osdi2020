#ifndef _TMPFS_H
#define _TMPFS_H

#include "vfs.h"

#define EOF (-1)
#define TMPFS_FILE_SIZE 512

void setup_fs_tmpfs(struct filesystem *fs);
struct vnode *create_tmpfs_vnode(int type);
int setup_mount_tmpfs(struct filesystem *fs, struct mount *mount);
int lookup_tmpfs(struct vnode *vnode, struct vnode **target,const char *component_name);
int create_tmpfs(struct vnode *vnode, struct vnode **target,const char *component_name);
// int write_tmpfs(struct file *file, const void *buf, unsigned len);
// int read_tmpfs(struct file *file, void *buf, unsigned len);
// void list_tmpfs(struct dentry *dir);

#endif//_TMPFS_H