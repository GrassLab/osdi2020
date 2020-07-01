#include "vfs.h"
#define BUFFER_MAX_LEN 200
struct tmpfs_filenode
{
    int file_size;
    char buffer[BUFFER_MAX_LEN];
};


int tmpfs_vnode_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);
int tmpfs_vnode_creat(struct vnode* dir_node, struct vnode** target, const char* component_name);
int tmpfs_write(struct file* file, const void* buf, int len);
int tmpfs_read(struct file* file, void* buf, int len);
int tmpfs_mount_setup(struct filesystem* fs, struct mount* mount);