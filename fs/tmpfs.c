#include <string.h>
#include <varied.h>
#include "tmpfs.h"

static struct vnode_operations _v_ops;
static struct vnode_operations *v_ops = &_v_ops;
static struct file_operations _f_ops;
static struct file_operations *f_ops = &_f_ops;

static struct vnode *internal_vnode_create (struct mount *mount,
					    enum tmpfs_type type);

static int
write (struct file *file, const void *buf, size_t len)
{
  struct tmpfs_node *node;
  size_t valid_len;
  node = file->vnode->internal;
  // type error
  if (node->type != file_t)
    return -1;
  // block full
  if (file->f_pos >= DATA_SIZE)
    return 0;
  valid_len = DATA_SIZE - file->f_pos;
  if (len < valid_len)
    valid_len = len;
  memcpy (&node->block->data.content[file->f_pos], buf, valid_len);
  file->f_pos += valid_len;
  if (node->block->size < file->f_pos)
    node->block->size = file->f_pos;
  return valid_len;
}

static int
read (struct file *file, void *buf, size_t len)
{
  struct tmpfs_node *node;
  size_t valid_len;
  node = file->vnode->internal;
  // type error
  if (node->type != file_t)
    return -1;
  // f_pos at EOF
  if (file->f_pos >= node->block->size)
    return 0;
  valid_len = node->block->size - file->f_pos;
  if (len < valid_len)
    valid_len = len;
  memcpy (buf, &node->block->data.content[file->f_pos], valid_len);
  file->f_pos += valid_len;
  return valid_len;
}

static int
lookup (struct vnode *dir_node, struct vnode **target,
	const char *component_name)
{
  struct tmpfs_node *node;
  struct tmpfs_node *sub_node;
  size_t i;
  node = dir_node->internal;
  if (node->type != dir_t)
    return 1;
  if (component_name[0] == '\0')
    return 1;
  for (i = 0; i < node->block->size; ++i)
    {
      sub_node = node->block->data.nodes[i];
      if (sub_node && !strcmp (component_name, sub_node->block->name))
	{
	  *target = sub_node->vnode;
	  return 0;
	}
    }
  return 1;
}

static int
create (struct vnode *dir_node, struct vnode **target,
	const char *component_name)
{
  struct tmpfs_node *node;
  struct vnode *new_vnode;
  size_t i;
  node = dir_node->internal;
  if (node->type != dir_t)
    return 1;
  if (component_name[0] == '\0')
    return 1;
  if (node->block->size >= SUBNODE_LEN)
    return 1;
  i = node->block->size;
  new_vnode = internal_vnode_create (dir_node->mount, file_t);
  if (new_vnode == NULL)
    return 1;
  node->block->data.nodes[i] = new_vnode->internal;
  // TODO: retrieve deleted slot
  ++node->block->size;
  node = new_vnode->internal;
  memcpy (node->block->name, component_name, strlen (component_name) + 1);
  *target = new_vnode;
  return 0;
}

static struct vnode *
internal_vnode_create (struct mount *mount, enum tmpfs_type type)
{
  struct vnode *new_node;
  struct tmpfs_node *internal;
  new_node = varied_malloc (sizeof (struct vnode));
  if (new_node == NULL)
    return NULL;
  new_node->mount = mount;
  new_node->f_ops = f_ops;
  new_node->v_ops = v_ops;
  new_node->internal = varied_malloc (sizeof (struct tmpfs_node));
  // TODO: memory leak
  if (new_node->internal == NULL)
    return NULL;
  internal = new_node->internal;
  internal->type = type;
  internal->vnode = new_node;
  internal->block = varied_malloc (sizeof (struct tmpfs_block));
  // TODO: memory leak
  if (internal->block == NULL)
    return NULL;
  return new_node;
}

static int
setup_mount (struct filesystem *fs, struct mount *mount)
{
  mount->fs = fs;
  mount->root = internal_vnode_create (mount, dir_t);
  if (mount->root == NULL)
    return 1;
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
