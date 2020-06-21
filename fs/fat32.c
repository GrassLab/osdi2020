#include <sdhost.h>
#include <string.h>
#include <varied.h>
#include "vfs.h"
#include "fat32.h"


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
  struct fat32_node *node;

  node = dir_node->internal;
  if (node->type != dir_t)
    return 1;
}

static int
create (struct vnode *dir_node, struct vnode **target,
	const char *component_name)
{
}

static struct vnode_operations _v_ops = {.create = create,.lookup = lookup };
static struct file_operations _f_ops = {.read = read,.write = write };

static struct vnode_operations *v_ops = &_v_ops;
static struct file_operations *f_ops = &_f_ops;

static struct fat32_node *
internal_node_create (struct fat32_node *parent, unsigned int cluster_index,
		      enum fat32_type type)
{
  struct fat32_node *new;
  new = varied_malloc (sizeof (*new));
  if (new == NULL)
    return NULL;
  new->info = parent->info;
  new->cluster_index = cluster_index;
  new->type = type;
  return new;
}

static int
setup_mount (struct filesystem *fs, struct mount *mount)
{
  // TODO: get device and fat32 offset by parameter
  char buf[512];
  struct partition_entry *part1;
  struct boot_sector *fat32;
  struct fat32_node init_node;
  // parse MBR to find first partition entry
  readblock (0, buf);
  part1 = (void *) buf;
  init_node.info.lba = part1->lba;
  init_node.info.size = part1->size;
  // parse fat32 BPB
  readblock (init_node.info.lba, buf);
  fat32 = (void *) buf;
  init_node.info.cluster_num_of_root = fat32->cluster_num_of_root;
  init_node.info.count_of_reserved = fat32->count_of_reserved;
  init_node.info.num_of_fat = fat32->num_of_fat;
  init_node.info.sectors_per_fat = fat32->sectors_per_fat;

  mount->fs = fs;
  mount->root = vnode_create (mount, v_ops, f_ops);
  if (mount->root == NULL)
    return 1;
  mount->root->internal =
    internal_node_create (&init_node, init_node.info.cluster_num_of_root,
			  dir_t);
  if (mount->root->internal == NULL)
    return 1;
  return 0;
}

void
fat32_init ()
{
  struct filesystem fs;
  fs.name = "fat32";
  fs.setup_mount = setup_mount;
  register_filesystem (&fs);
}
