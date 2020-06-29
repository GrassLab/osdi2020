#ifndef __TMPFS__
#define __TMPFS__

#include "vfs.h"

#define EOF (-1)
#define TMP_FILE_SIZE 512

struct tmpfs_internal {
    int size; //TODO
    char buf[TMP_FILE_SIZE];
};

int tmpfs_register();
int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount);

// vnode operations
int tmpfs_lookup(struct vnode* dir, struct vnode** target, const char* component_name);
int tmpfs_create(struct vnode* dir, struct vnode** target, const char* component_name);
int tmpfs_ls(struct vnode* dir);
int tmpfs_mkdir(struct vnode* dir, struct vnode** target, const char* component_name);

// file operations
int tmpfs_read(struct file* file, void* buf, uint64_t len);
int tmpfs_write(struct file* file, const void* buf, uint64_t len);

#endif // __TMPFS__
