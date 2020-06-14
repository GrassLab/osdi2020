#include "vfs.h"
#ifndef __PROCFS_H__
#define __PROCFS_H__

struct vfs_filesystem_struct * procfs_init(void);
int procfs_setup_mount(struct vfs_filesystem_struct * fs, struct vfs_mount_struct * mount);
int procfs_mount(struct vfs_vnode_struct * mountpoint_vnode, struct vfs_mount_struct * mount);
int procfs_umount(struct vfs_vnode_struct * mountpoint_parent, const char * mountpoint_token);
int procfs_lookup(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name);
int procfs_create(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name);
int procfs_write(struct vfs_file_struct * file, const void * buf, size_t len);
int procfs_read(struct vfs_file_struct * file, void * buf, size_t len);
int procfs_list(struct vfs_vnode_struct * dir_node);
int procfs_mkdir(struct vfs_vnode_struct * dir_node, const char * new_dir_name);

#endif

