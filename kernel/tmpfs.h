#ifndef KERNEL_TMPFS_H_
#define KERNEL_TMPFS_H_

#include "kernel/lib/types.h"
#include "kernel/vfs.h"

#define MAX_FILE_SIZE 512
#define MAX_FILE_PER_DIR 16
#define MAX_FILENAME_LEN 256

struct filesystem tmpfs;
struct file_operations tmpfs_file_operations;
struct vnode_operations tmpfs_vnode_operations;

struct tmpfs_file {
  struct vnode *vn;
  size_t size;
  char name[MAX_FILENAME_LEN];
  unsigned char contents[MAX_FILE_SIZE];
};

struct tmpfs_dir {
  struct vnode *vn;
  size_t file_nums;
  struct tmpfs_file files[MAX_FILE_PER_DIR];
  size_t d_pos; // The index of next file information to be read
};

int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount);

int tmpfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);
int tmpfs_create(struct vnode* dir_node, struct vnode** target, const char* component_name);

int tmpfs_write(struct file* file, const void* buf, size_t len);
int tmpfs_read(struct file* file, void* buf, size_t len);
int tmpfs_readdir(struct file* dir, char* buf, size_t len);

#endif // KERNEL_TMPFS_H_
