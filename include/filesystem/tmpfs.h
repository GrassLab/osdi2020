#ifndef _TMPFS_H
#define _TMPFS_H

#include "type.h"
#include "filesystem/filesystem.h"

#define MAX_DIR_CHILD 100

struct dentry
{
    struct vnode* vnode;
    struct vnode** child;
    size_t child_num;
};

int32_t tmpfsSetupMount(struct filesystem* fs, struct mount* mount);

#endif