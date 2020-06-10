#include "tmpfs.h"
#include "slab.h"
#include "meta_macro.h"

struct vfs_filesystem_struct * tmpfs_init(void)
{
  /* should be called only once */
  struct vfs_filesystem_struct * fs = (struct vfs_filesystem_struct *)slab_malloc(sizeof(struct vfs_filesystem_struct));

  fs -> name = "tmpfs";
  fs -> setup_mount = tmpfs_mount;

  return fs;
}

int tmpfs_mount(struct vfs_filesystem_struct * fs, struct vfs_mount_struct * mount)
{
  UNUSED(fs);

  /* create root directory */
  struct vfs_vnode_struct * root_dir_vnode = (struct vfs_vnode_struct *)slab_malloc(sizeof(struct vfs_vnode_struct));

  root_dir_vnode -> mount = mount;
  root_dir_vnode -> internal = (void *)tmpfs_create_dir_node("/");
  mount -> root = root_dir_vnode;
  return 0;
}

struct tmpfs_dir_node * tmpfs_create_dir_node(char * dir_name)
{
  struct tmpfs_dir_node * dir_node = (struct tmpfs_dir_node *)slab_malloc(sizeof(struct tmpfs_dir_node));

  dir_node -> name = dir_name;

  /* no subdir and files when created */
  for(unsigned idx = 0; idx < TMPFS_MAX_SUB_DIR; ++idx)
  {
    (dir_node -> subdir_node)[idx] = 0;
  }
  for(unsigned idx = 0; idx < TMPFS_MAX_FILE_IN_DIR; ++idx)
  {
    (dir_node -> files)[idx] = 0;
  }

  return dir_node;
}

