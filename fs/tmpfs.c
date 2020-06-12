#include "tmpfs.h"

static int
setup_mount (struct filesystem *fs, struct mount *mount)
{
  mount->fs = fs;
  return 0;
}

void
tmpfs_init ()
{
  struct filesystem fs;
  fs.name = "tmpfs";
  fs.setup_mount = setup_mount;
  register_filesystem (&fs);
}
