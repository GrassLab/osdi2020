#ifndef __TMPFS_H__
#define __TMPFS_H__

#include "fs.h"

extern struct filesystem *tmpfs;
void tmpfs_show_tree(struct vnode *node, int);

#endif
