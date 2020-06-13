#include <string.h>
#include <varied.h>
#include "tmpfs.h"

static struct vnode_operations _v_ops;
static struct vnode_operations *v_ops = &_v_ops;
static struct file_operations _f_ops;
static struct file_operations *f_ops = &_f_ops;

static int
write (struct file *file, const void *buf, size_t len)
{
}

static int
read (struct file *file, void *buf, size_t len)
{
}

static int
lookup (struct vnode *dir_node, struct vnode **target,
	const char *component_name)
{
}

static int
create (struct vnode *dir_node, struct vnode **target,
	const char *component_name)
{
}

static struct vnode *
vnode_create (struct mount *mount)
{
  struct vnode *new_node;
  new_node = varied_malloc (sizeof (struct vnode));
  if (new_node == NULL)
    {
      // TODO: handle malloc fail
      printf ("vnode_create: not enough memory");
      while (1);
    }
  new_node->mount = mount;
  new_node->f_ops = f_ops;
  new_node->v_ops = v_ops;
  new_node->internal = varied_malloc (sizeof (struct tmpfs_block));
  return new_node;
}

static int
setup_mount (struct filesystem *fs, struct mount *mount)
{
  mount->fs = fs;
  mount->root = vnode_create (mount);
  return 0;
}

void
tmpfs_init ()
{
  struct filesystem fs;
  fs.name = "tmpfs";
  fs.setup_mount = setup_mount;
  register_filesystem (&fs);
  v_ops->create = create;
  v_ops->lookup = lookup;
  f_ops->read = read;
  f_ops->write = write;
}
