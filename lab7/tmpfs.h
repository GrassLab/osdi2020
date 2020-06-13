#include "vfs.h"
#include <stdint.h>
#ifndef __TMPFS_H__
#define __TMPFS_H__

#define TMPFS_MAX_SUB_DIR 4
#define TMPFS_MAX_FILE_IN_DIR 4
#define TMPFS_MAX_NAME 0x20
#define TMPFS_MAX_FILE_SIZE 0x100

struct tmpfs_dir_node
{
  char name[TMPFS_MAX_NAME];
  struct vfs_mount_struct * mountpoint;
  struct tmpfs_dir_node * parent_node;
  struct tmpfs_dir_node * subdir_node[TMPFS_MAX_SUB_DIR];
  struct tmpfs_file_node * files[TMPFS_MAX_FILE_IN_DIR];
};

struct tmpfs_file_node
{
  char name[TMPFS_MAX_NAME];
  unsigned file_size;
  uint64_t * location;
};

struct vfs_filesystem_struct * tmpfs_init(void);
int tmpfs_setup_mount(struct vfs_filesystem_struct * fs, struct vfs_mount_struct * mount);
int tmpfs_mount(struct vfs_vnode_struct * mountpoint_vnode, struct vfs_mount_struct * mount);
int tmpfs_lookup(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name);
int tmpfs_create(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name);
int tmpfs_write(struct vfs_file_struct * file, const void * buf, size_t len);
int tmpfs_read(struct vfs_file_struct * file, void * buf, size_t len);
int tmpfs_list(struct vfs_vnode_struct * dir_node);
int tmpfs_mkdir(struct vfs_vnode_struct * dir_node, const char * new_dir_name);
struct tmpfs_dir_node * tmpfs_create_dir_node(const char * dir_name);

#endif

