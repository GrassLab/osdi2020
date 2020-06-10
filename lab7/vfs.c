#include "vfs.h"

static struct vfs_mount_struct * rootfs;

int vfs_regist_fs(struct vfs_filesystem_struct * fs)
{
  /* register the file system to the kernel.
   * you can also initialize memory pool of the file system here.
   */
  /* TODO */
  return 0;
}

struct vfs_file_struct * vfs_open(const char * pathname, int flags)
{
  /*
   * 1. Lookup pathname from the root vnode.
   * 2. Create a new file descriptor for this vnode if found.
   * 3. Create a new file if O_CREAT is specified in flags.
   */
  /*TODO */
  return 0;
}

int vfs_close(struct vfs_file_struct * file)
{
  /* 1. release the file descriptor */

  /* TODO */
  return 0;
}

int vfs_write(struct vfs_file_struct * file, const void * buf, size_t len)
{
  /* 1. write len byte from buf to the opened file.
   * 2. return written size or error code if an error occurs.
   */

  /* TODO */
  return 0;
}

int vfs_read(struct vfs_file_struct * file, void * buf, size_t len)
{
  /* 1. read min(len, readable file data size) byte to buf from the opened file.
   * 2. return read size or error code if an error occurs.
   */
  /* TODO */
  return 0;
}

