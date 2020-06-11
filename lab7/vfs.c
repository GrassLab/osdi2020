#include "vfs.h"
#include "string_util.h"
#include "slab.h"
#include "uart.h"

static struct vfs_mount_struct * rootfs;
static struct vfs_filesystem_struct * fs_list[VFS_MAX_REGISTERED_FS];

int vfs_regist_fs(struct vfs_filesystem_struct * fs)
{
  /* put fs into list */
  for(unsigned idx = 0; idx < VFS_MAX_REGISTERED_FS; ++idx)
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
  struct vfs_vnode_struct * file_vnode = vfs_traverse(pathname);
  struct vfs_file_struct * file;

  if(file_vnode == 0)
  {
    if((flags & O_CREAT) == 0)
    {
      return 0;
    }
    /* create file */
    /* todo: hierachical */
    (rootfs -> root -> v_ops -> create)(rootfs -> root, &file_vnode, pathname + 1);
  }

  file = (struct vfs_file_struct *)slab_malloc(sizeof(struct vfs_file_struct));
  file -> vnode = file_vnode;
  file -> f_pos = 0;
  file -> f_ops = file_vnode -> f_ops;
  file -> flags = flags;

  return file;
}

int vfs_close(struct vfs_file_struct * file)
{
  slab_malloc_free((uint64_t *)(file -> vnode));
  slab_malloc_free((uint64_t *)file);
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

struct vfs_vnode_struct * vfs_traverse(const char * pathname)
{
  /* return 0 if none found */
  /* TODO: Now, Single file system with no mount point */

  struct vfs_vnode_struct * target;
  unsigned path_length = (unsigned)string_length(pathname);
  unsigned search_start = 0;

  /* if(path_length == 1 && pathname[0] == '/') */
  /* pathname == "/", which impossible to open */
  if(pathname[0] == '/')
  {
    /* absolute path */
    search_start = 1;
  }

  while(search_start < path_length)
  {
    char component_name[VFS_MAX_COMPONENET_NAME_LENGTH];
    unsigned component_length = string_split(pathname + search_start, '/');

    memcopy(pathname + search_start, component_name, component_length);
    /* add trailing '\0' to string after memcopy */
    component_name[component_length] = '\0';
    /* ignore '/' */
    search_start += component_length + 1;

    /* search componenet in the current directory */
    (rootfs -> root -> v_ops -> lookup)(rootfs -> root, &target, component_name);
  }

  return target;
}

