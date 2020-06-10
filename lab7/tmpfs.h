#include "vfs.h"
#ifndef __TMPFS_H__
#define __TMPFS_H__

struct vfs_filesystem_struct * tmpfs_init(void);
int tmpfs_mount(struct vfs_filesystem_struct * fs, struct vfs_mount_struct * mount);

#endif

