#include "vfs.h"
#include "string_util.h"
#include "slab.h"
#include "uart.h"

static struct vfs_mount_struct * rootfs;
static struct vfs_filesystem_struct * fs_list[MAX_REGISTERED_FS];

int vfs_regist_fs(struct vfs_filesystem_struct * fs)
{
  /* put fs into list */
  for(unsigned idx = 0; idx < MAX_REGISTERED_FS; ++idx)
  {
    if(fs_list[idx] == 0)
    {
      fs_list[idx] = fs;
    }
  }

  uart_puts(fs -> name);
  uart_puts(" registered\n");

  /* point to rootfs if tmpfs */
  if(string_cmp(fs -> name, "tmpfs", 8) != 0)
  {
    vfs_set_tmpfs_to_rootfs(fs);
  }

  return 0;
}

void vfs_set_tmpfs_to_rootfs(struct vfs_filesystem_struct * fs)
{
  /* WARNING: Currently this function should be called once */

  /* allocate vfs_mount_struct then call tmpfs mount setup */
  struct vfs_mount_struct * mount = (struct vfs_mount_struct *)slab_malloc(sizeof(struct vfs_mount_struct));
  mount -> fs = fs;
  (fs -> setup_mount)(fs, mount);

  rootfs = mount;
  return;
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

