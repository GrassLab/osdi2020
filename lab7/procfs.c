#include "procfs.h"
#include "slab.h"
#include "meta_macro.h"
#include "uart.h"

static struct vfs_vnode_operations_struct * procfs_vnode_ops;
static struct vfs_file_operations_struct * procfs_file_ops;
static struct procfs_root_struct procfs_root;

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
  procfs_vnode_ops -> post_mount = procfs_post_mount;
  procfs_file_ops -> write = procfs_write;
  procfs_file_ops -> read = procfs_read;

  vfs_regist_fs(fs);

  return fs;
}

int procfs_setup_mount(struct vfs_filesystem_struct * fs, struct vfs_mount_struct * mount)
{
  UNUSED(fs);

  struct vfs_vnode_struct * root_dir_vnode = procfs_create_vnode(mount, (void *)&procfs_root, 1);
  procfs_root.switch_ = 0;
  mount -> root = root_dir_vnode;

  return 0;

}

int procfs_post_mount(struct vfs_vnode_struct * mountpoint_parent, struct vfs_mount_struct * mount)
{
  ((struct procfs_root_struct *)(mount -> root -> internal)) -> parent = mountpoint_parent;
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
  /* return 0 if node should be free after use, 1 if not */
  if(dir_node -> internal == (void *)&procfs_root)
  {
    if(string_cmp("switch", component_name, 999) != 0)
    {
      *target = procfs_create_vnode(dir_node -> mount, (void *)PROCFS_TYPE_SWITCH, 0);
    }
    else if(string_cmp("hello", component_name, 999) != 0)
    {
      *target = procfs_create_vnode(dir_node -> mount, (void *)PROCFS_TYPE_HELLO, 0);
    }
    else /* task */
    {
    }
  }
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

struct vfs_vnode_struct * procfs_create_vnode(struct vfs_mount_struct * mount, void * internal, int is_dir)
{
  struct vfs_vnode_struct * target_vnode = (struct vfs_vnode_struct *)slab_malloc(sizeof(struct vfs_vnode_struct));
  target_vnode -> mount = mount;
  target_vnode -> v_ops = procfs_vnode_ops;
  target_vnode -> f_ops = procfs_file_ops;
  target_vnode -> internal = internal;
  target_vnode -> is_dir = is_dir;
  return target_vnode;
}

