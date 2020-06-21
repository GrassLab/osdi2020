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
  struct file *fd;
  struct vnode *dir_node;
  const char *pos;
  int i;
  char component[16];
  dir_node = rootfs->root;
  pos = pathname;
  // strip leading
  while (*pos == '/')
    ++pos;
  // 1. Lookup pathname from the root vnode.
  // TODO: handle buffer overflow, use i % 16 now :p
  i = 0;
  while (*pos != '\0')
    {
      if (*pos == '/')
	{
	  component[i % 16] = '\0';
	  i = 0;
	  if (dir_node->v_ops->lookup (dir_node, &dir_node, component))
	    return NULL;
	}
      else
	{
	  component[i % 16] = *pos;
	  ++i;
	}
      ++pos;
    }
  if (i != 0)
    {
      component[i % 16] = '\0';
      if (dir_node->v_ops->lookup (dir_node, &dir_node, component))
	{
	  if ((flags & O_CREAT))
	    {
	      // 3. Create a new file if O_CREAT is specified in flags.
	      if (dir_node->v_ops->create (dir_node, &dir_node, component))
		return NULL;
	    }
	  else
	    return NULL;
	}
    }
  // 2. Create a new file descriptor for this vnode if found.
  fd = varied_malloc (sizeof (*fd));
  if (fd == NULL)
    return NULL;
  fd->vnode = dir_node;
  fd->flags = flags;
  fd->f_pos = 0;
  fd->f_ops = fd->vnode->f_ops;
  return fd;
}

int
vfs_close (struct file *file)
{
  // 1. release the file descriptor
  varied_free (file);
  return 0;
}

int
vfs_write (struct file *file, const void *buf, size_t len)
{
  // 1. write len byte from buf to the opened file.
  // 2. return written size or error code if an error occurs.
  return file->f_ops->write (file, buf, len);
}

int
vfs_read (struct file *file, void *buf, size_t len)
{
  // 1. read min(len, readable file data size) byte to buf from the opened file.
  // 2. return read size or error code if an error occurs.
  return file->f_ops->read (file, buf, len);
}

void
rootfs_init ()
{
  int i;
  // simply init rootfs's filesystem
  extern void sd_init ();
  extern void fat32_init ();
  sd_init ();
  fat32_init ();
  for (i = 0; i < FS_NUM; ++i)
    if (!strcmp ("fat32", registed_fs[i].name))
      break;
  rootfs = varied_malloc (sizeof (*rootfs));
  registed_fs[i].setup_mount (&registed_fs[i], rootfs);
}
