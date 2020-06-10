#include "vfs.h"
#include <stdint.h>
#ifndef __TMPFS_H__
#define __TMPFS_H__

#define TMPFS_MAX_SUB_DIR 4
#define TMPFS_MAX_FILE_IN_DIR 4

struct tmpfs_dir_node
{
  char * name;
  struct tmpfs_dir_node * subdir_node[TMPFS_MAX_SUB_DIR];
  struct tmpfs_file_node * files[TMPFS_MAX_FILE_IN_DIR];
};

struct tmpfs_file_node
{
  char * name;
  uint8_t file_size;
  uint64_t * location;
};

struct vfs_filesystem_struct * tmpfs_init(void);
int tmpfs_mount(struct vfs_filesystem_struct * fs, struct vfs_mount_struct * mount);
struct tmpfs_dir_node * tmpfs_create_dir_node(char * dir_name);

#endif

