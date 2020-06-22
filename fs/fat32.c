#include <sdhost.h>
#include <string.h>
#include <varied.h>
#include "vfs.h"
#include "fat32.h"

#define BLOCK_SIZE 512
#define CHAIN_LEN (BLOCK_SIZE / sizeof (unsigned int))
#define CHAIN_EOF 0xffffff8
#define DIR_LEN (BLOCK_SIZE / sizeof (struct directory_entry))

static int write (struct file *file, const void *buf, size_t len);
static int read (struct file *file, void *buf, size_t len);
static int create (struct vnode *dir_node, struct vnode **target,
		   const char *component_name);
static int lookup (struct vnode *dir_node, struct vnode **target,
		   const char *component_name);
static struct fat32_node *internal_node_create (struct fat32_node *parent,
						unsigned int cluster_index,
						unsigned int dir_entry,
						unsigned int dir_index);

static struct vnode_operations _v_ops = {.create = create,.lookup = lookup };
static struct file_operations _f_ops = {.read = read,.write = write };

static struct vnode_operations *v_ops = &_v_ops;
static struct file_operations *f_ops = &_f_ops;

// TODO: cache
static unsigned int
fat32_cluster_value (struct fat32_info *info, unsigned int index)
{
  unsigned int offset;
  unsigned int fat[CHAIN_LEN];

  // find FAT
  offset = info->lba + info->count_of_reserved;
  offset += index / CHAIN_LEN;
  readblock (offset, fat);
  return fat[index % CHAIN_LEN];
}

static int
fat32_node_dir_entry (struct fat32_node *node,
		      struct directory_entry *dir_entry)
{
  struct directory_entry dirs[DIR_LEN];
  if (node->dir_index >= DIR_LEN)
    return 1;
  if (node->cluster_index == node->info.cluster_num_of_root)
    {
      dir_entry->size = 0;
      return 0;
    }
  readblock (node->info.lba + node->dir_entry, dirs);
  *dir_entry = dirs[node->dir_index];
  return 0;
}

static int
write (struct file *file, const void *buf, size_t len)
{
}

static int
read (struct file *file, void *buf, size_t len)
{
}

static int
filename_cmp (struct directory_entry *dir, const char *filename)
{
  int name_ind, ext_ind;
  const char *extname;
  for (name_ind = 0; name_ind < 8; ++name_ind)
    {
      if (filename[name_ind] == '\0')
	return dir->name[name_ind] != '\x20';
      if (filename[name_ind] == '.')
	{
	  if (dir->name[name_ind] != '\x20')
	    return 1;
	  break;
	}
      if (filename[name_ind] != dir->name[name_ind])
	return 1;
    }
  if (filename[name_ind] == '\0')
    return 0;
  extname = &filename[name_ind + 1];
  for (ext_ind = 0; ext_ind < 3; ++ext_ind)
    {
      if (extname[ext_ind] == '\0')
	return dir->extension[ext_ind] != '\x20';
      if (extname[ext_ind] != dir->extension[ext_ind])
	return 1;
    }
  return 0;

  if (dir->name[name_ind] != '\x20')
    return 1;
  if (filename[name_ind] == '\0' && dir->name[name_ind] == '\x20'
      && dir->extension[0] == '\x20')
    return 0;
  ++name_ind;
  for (ext_ind = 0; ext_ind < 3; ++ext_ind)
    {
      if (filename[name_ind + ext_ind] == '\0')
	break;
    }
}

static int
lookup (struct vnode *dir_node, struct vnode **target,
	const char *component_name)
{
  struct vnode *new_vnode;
  struct fat32_node *node;
  struct directory_entry dir;
  struct directory_entry dirs[DIR_LEN];
  unsigned int offset;
  unsigned int value;
  unsigned int i;

  // verify node type
  node = dir_node->internal;
  if (fat32_node_dir_entry (node, &dir))
    return 1;
  // directory check
  if (dir.size != 0)
    return 1;
  // traversal all block to find target
  // find block offset of fat cluster chain
  value = node->cluster_index;
  // find offset of data region
  offset = node->info.lba + node->info.count_of_reserved;
  offset += node->info.num_of_fat * node->info.sectors_per_fat;
  while ((value & CHAIN_EOF) != CHAIN_EOF)
    {
      // read block
      readblock (offset + value - node->info.cluster_num_of_root, dirs);
      // search component
      for (i = 0; i < DIR_LEN; ++i)
	{
	  if (!filename_cmp (&dirs[i], component_name))
	    {
	      new_vnode = vnode_create (dir_node->mount, v_ops, f_ops);
	      new_vnode->internal = internal_node_create (node,
							  (dirs[i].start_hi <<
							   16) +
							  dirs[i].start_lo,
							  value, i);
	      *target = new_vnode;
	      return 0;
	    }
	}
      value = fat32_cluster_value (&node->info, value);
    }
  return 1;
}

static int
create (struct vnode *dir_node, struct vnode **target,
	const char *component_name)
{
}

static struct fat32_node *
internal_node_create (struct fat32_node *parent, unsigned int cluster_index,
		      unsigned int dir_entry, unsigned int dir_index)
{
  struct fat32_node *new;
  new = varied_malloc (sizeof (*new));
  if (new == NULL)
    return NULL;
  new->info = parent->info;
  new->cluster_index = cluster_index;
  new->dir_entry = dir_entry;
  new->dir_index = dir_index;
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
			  0, 0);
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
