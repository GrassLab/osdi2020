#include "meta_macro.h"
#include "sys.h"
#include "uart.h"
#include "string_util.h"
#include "mmu.h"
#include "slab.h"
#include "vfs.h"

int sys_mkdir(const char * pathname)
{
  vfs_mkdir(vfs_traverse(pathname, VFS_TRAVERSE_RETURN_NEAREST), pathname);
  return 0;
}

int sys_chdir(const char * pathname)
{
  vfs_chdir(vfs_traverse(pathname, VFS_TRAVERSE_NO_RETURN_NEAREST));
  return 0;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
int sys_mount(const char * device, const char * mountpoint, const char * filesystem)
{
  /* check if the filesystem is registered */
  struct vfs_filesystem_struct * fs = vfs_get_fs(filesystem);
  if(fs == 0)
  {
    uart_puts("FS not found\n");
    return 1;
  }

  /* find mountpoint */
  struct vfs_vnode_struct * mountpoint_vnode = vfs_traverse(mountpoint, VFS_TRAVERSE_NO_RETURN_NEAREST);

  /* setup new mount */
  struct vfs_mount_struct * mount;
  vfs_setup_mount(fs, &mount);

  /* mount onto mountoint */
  vfs_mount(mountpoint_vnode, mount);

  return 0;
}

int sys_umount(const char * mountpoint)
{
  char dir_name[0x80];
  char parent_dir_name[0x80];
  string_copy(mountpoint, dir_name);
  string_copy(mountpoint, parent_dir_name);
  string_concat(parent_dir_name, "/..");
  vfs_last_token_in_path(dir_name);
  vfs_umount(vfs_traverse(parent_dir_name, VFS_TRAVERSE_NO_RETURN_NEAREST), dir_name);
  return 0;
}

