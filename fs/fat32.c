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

static int
setup_mount (struct filesystem *fs, struct mount *mount)
{
}

void
fat32_init ()
{
  struct filesystem fs;
  fs.name = "fat32";
  fs.setup_mount = setup_mount;
  register_filesystem (&fs);
}
