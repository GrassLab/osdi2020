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
  mount -> root = root_dir_vnode;
  return 0;
}

