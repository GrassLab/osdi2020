#include <stddef.h>
#ifndef __VFS_H__
#define __VFS_H__

#define VFS_MAX_REGISTERED_FS 8
#define VFS_MAX_MOUNT_POINT 8
#define VFS_MAX_COMPONENET_NAME_LENGTH 0x20

#define VFS_TRAVERSE_NO_RETURN_NEAREST 0
#define VFS_TRAVERSE_RETURN_NEAREST 1

#define O_CREAT 00000100

struct vfs_vnode_struct
{
  struct vfs_mount_struct * mount;
  struct vfs_vnode_operations_struct * v_ops;
  struct vfs_file_operations_struct * f_ops;
  int is_dir;
  void * internal;
};

struct vfs_file_struct
{
  struct vfs_vnode_struct * vnode;
  size_t read_pos;
  size_t write_pos;
  struct vfs_file_operations_struct * f_ops;
  int flags;
};

struct vfs_mount_struct
{
  struct vfs_vnode_struct * root;
  struct vfs_filesystem_struct * fs;
};

struct vfs_filesystem_struct
{
  const char * name;
  int (*setup_mount)(struct vfs_filesystem_struct * fs, struct vfs_mount_struct * mount);
};

struct vfs_file_operations_struct
{
  int (*write) (struct vfs_file_struct * file, const void * buf, size_t len);
  int (*read) (struct vfs_file_struct * file, void * buf, size_t len);
};

struct vfs_vnode_operations_struct
{
  int (*lookup)(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name);
  int (*create)(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name);
  int (*list)(struct vfs_vnode_struct * dir_node);
  int (*mkdir)(struct vfs_vnode_struct * dir_node, const char * new_dir_name);
  int (*mount)(struct vfs_vnode_struct * mountpoint_vnode, struct vfs_mount_struct * mount);
};


int vfs_regist_fs(struct vfs_filesystem_struct * fs);
void vfs_setup_mount(struct vfs_filesystem_struct * fs, struct vfs_mount_struct ** mount);
void vfs_set_tmpfs_to_rootfs(struct vfs_filesystem_struct * fs);
void vfs_mount(struct vfs_vnode_struct * mountpoint, struct vfs_mount_struct * mount);
struct vfs_file_struct * vfs_open(const char * pathname, int flags);
int vfs_close(struct vfs_file_struct * file);
int vfs_write(struct vfs_file_struct * file, const void * buf, size_t len);
int vfs_read(struct vfs_file_struct * file, void * buf, size_t len);
int vfs_list(struct vfs_file_struct * file);
int vfs_mkdir(struct vfs_vnode_struct * current_dir_vnode, const char * dir_name);
int vfs_chdir(struct vfs_vnode_struct * target_dir_vnode);
struct vfs_filesystem_struct * vfs_get_fs(const char * name);

struct vfs_vnode_struct * vfs_traverse(const char * pathname, int return_closest_node);

struct vfs_vnode_struct * vfs_get_root_vnode(void);
void vfs_last_token_in_path(char * string);
void vfs_free_vnode(struct vfs_vnode_struct * vnode);

#endif

