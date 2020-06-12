#include <string.h>
#include <varied.h>
#include "vfs.h"

#define FS_NUM 16
struct filesystem registed_fs[FS_NUM];

int
register_filesystem (struct filesystem *fs)
{
  // register the file system to the kernel.
  // you can also initialize memory pool of the file system here.
  int i;
  for (i = 0; i < FS_NUM; ++i)
  {
    if (registed_fs[i].name == NULL)
      {
	registed_fs[i] = *fs;
	break;
      }
  }
  return i == FS_NUM;
}

struct file *
vfs_open (const char *pathname, int flags)
{
  // 1. Lookup pathname from the root vnode.
  // 2. Create a new file descriptor for this vnode if found.
  // 3. Create a new file if O_CREAT is specified in flags.
}

int
vfs_close (struct file *file)
{
  // 1. release the file descriptor
}

int
vfs_write (struct file *file, const void *buf, size_t len)
{
  // 1. write len byte from buf to the opened file.
  // 2. return written size or error code if an error occurs.
}

int
vfs_read (struct file *file, void *buf, size_t len)
{
  // 1. read min(len, readable file data size) byte to buf from the opened file.
  // 2. return read size or error code if an error occurs.
}

void
rootfs_init ()
{
  int i;
  struct mount *mount;
  // simply init rootfs's filesystem
  extern void tmpfs_init ();
  tmpfs_init ();
  for (i = 0; i < FS_NUM; ++i)
    if (!strcmp ("tmpfs", registed_fs[i].name))
      break;
  rootfs = varied_malloc (sizeof (mount));
  registed_fs[i].setup_mount (&registed_fs[i], rootfs);
}
