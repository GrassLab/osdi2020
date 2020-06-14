#include "procfs.h"
#include "slab.h"
#include "meta_macro.h"
#include "uart.h"

static struct vfs_vnode_operations_struct * procfs_vnode_ops;
static struct vfs_file_operations_struct * procfs_file_ops;

struct vfs_filesystem_struct * procfs_init(void)
{
  /* should be called only once */
  struct vfs_filesystem_struct * fs = (struct vfs_filesystem_struct *)slab_malloc(sizeof(struct vfs_filesystem_struct));

  fs -> name = "procfs";
  fs -> setup_mount = procfs_setup_mount;

  procfs_vnode_ops = (struct vfs_vnode_operations_struct *)slab_malloc(sizeof(struct vfs_vnode_operations_struct));
  procfs_file_ops = (struct vfs_file_operations_struct *)slab_malloc(sizeof(struct vfs_file_operations_struct));

  procfs_vnode_ops -> lookup = procfs_lookup;
  procfs_vnode_ops -> create = procfs_create;
  procfs_vnode_ops -> list = procfs_list;
  procfs_vnode_ops -> mkdir = procfs_mkdir;
  procfs_vnode_ops -> mount = procfs_mount;
  procfs_vnode_ops -> umount = procfs_umount;
  procfs_file_ops -> write = procfs_write;
  procfs_file_ops -> read = procfs_read;

  vfs_regist_fs(fs);

  return fs;
}

int procfs_setup_mount(struct vfs_filesystem_struct * fs, struct vfs_mount_struct * mount)
{
  UNUSED(fs);

  struct vfs_vnode_struct * root_dir_vnode = 0;

  mount -> root = root_dir_vnode;

  return 0;

}

int procfs_mount(struct vfs_vnode_struct * mountpoint_vnode, struct vfs_mount_struct * mount)
{
  UNUSED(mountpoint_vnode);
  UNUSED(mount);
  uart_puts_blocking("procfs_mount not implemented");
  while(1);
  return 0;
}

int procfs_umount(struct vfs_vnode_struct * mountpoint_parent, const char * mountpoint_token)
{
  UNUSED(mountpoint_parent);
  UNUSED(mountpoint_token);
  uart_puts_blocking("procfs_umount not implemented");
  while(1);
  return 0;
}

int procfs_lookup(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name)
{
  return 0;
}

int procfs_create(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name)
{
  UNUSED(dir_node);
  UNUSED(target);
  UNUSED(component_name);
  uart_puts_blocking("procfs_create not implemented");
  while(1);
  return 0;
}

int procfs_write(struct vfs_file_struct * file, const void * buf, size_t len)
{
  return 0;
}

int procfs_read(struct vfs_file_struct * file, void * buf, size_t len)
{
  return 0;
}

int procfs_list(struct vfs_vnode_struct * dir_node)
{
  UNUSED(dir_node);
  uart_puts_blocking("procfs_list not implemented");
  while(1);
  return 0;
}

int procfs_mkdir(struct vfs_vnode_struct * dir_node, const char * new_dir_name)
{
  UNUSED(dir_node);
  UNUSED(new_dir_name);
  uart_puts_blocking("procfs_mkdir not implemented");
  while(1);
  return 0;
}

