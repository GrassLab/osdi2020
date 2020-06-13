#ifndef TMPFS
#define TMPFS

#include "vfs.h"

#define BLOCK_SIZE 512
#define NAME_LEN 16
#define DATA_SIZE (BLOCK_SIZE - NAME_LEN)
#define SUBNODE_LEN (DATA_SIZE / sizeof (struct tmpfs_node *))

enum tmpfs_type
{
  dir_t,
  file_t
};

union tmpfs_data
{
  char content[DATA_SIZE];
  struct tmpfs_node *nodes[SUBNODE_LEN];
};

struct tmpfs_block
{
  char name[NAME_LEN];
  union tmpfs_data data;
};

struct tmpfs_node
{
  enum tmpfs_type type;
  struct tmpfs_block *block;
  struct vnode *vnode;
};

void tmpfs_init ();
#endif /* ifndef TMPFS */
