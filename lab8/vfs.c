#include "vfs.h"
#include "string_util.h"
#include "slab.h"
#include "uart.h"

static struct vfs_mount_struct * rootfs;
static struct vfs_vnode_struct * root_vnode;
static struct vfs_filesystem_struct * fs_list[VFS_MAX_REGISTERED_FS];
static struct vfs_vnode_struct * mountpoints_vnode[VFS_MAX_MOUNT];

static struct vfs_vnode_struct * single_thread_current_dir_vnode;

int vfs_regist_fs(struct vfs_filesystem_struct * fs)
{
  /* put fs into list */
  for(unsigned idx = 0; idx < VFS_MAX_REGISTERED_FS; ++idx)
  {
    if(fs_list[idx] == 0)
    {
      fs_list[idx] = fs;
      break;
    }
  }

  uart_puts(fs -> name);
  uart_puts(" registered\n");

  return 0;
}

void vfs_setup_mount(struct vfs_filesystem_struct * fs, struct vfs_mount_struct ** mount)
{
  /* allocate vfs_mount_struct then call tmpfs mount setup */
  *mount = (struct vfs_mount_struct *)slab_malloc(sizeof(struct vfs_mount_struct));
  (*mount) -> fs = fs;
  (fs -> setup_mount)(fs, *mount);
  return;
}

void vfs_set_rootfs(struct vfs_filesystem_struct * fs)
{
  /* WARNING: Currently this function should be called once */

  vfs_setup_mount(fs, &rootfs);
  root_vnode = rootfs -> root;
  single_thread_current_dir_vnode = root_vnode;
  return;
}

void vfs_mount(struct vfs_vnode_struct * mountpoint, struct vfs_mount_struct * mount)
{
  (mountpoint -> v_ops -> mount)(mountpoint, mount);
  for(int i = 0; i < VFS_MAX_MOUNT; ++i)
  {
    if(mountpoints_vnode[i] == 0)
    {
      mountpoints_vnode[i] = mount -> root;
    }
  }
  return;
}

void vfs_umount(struct vfs_vnode_struct * mountpoint_parent, const char * mountpoint_token)
{
  struct vfs_vnode_struct * mount_vnode;
  (mountpoint_parent -> v_ops -> lookup)(mountpoint_parent, &mount_vnode, mountpoint_token);
  (mountpoint_parent -> v_ops -> umount)(mountpoint_parent, mountpoint_token);
  for(int i = 0; i < VFS_MAX_MOUNT; ++i)
  {
    if(mountpoints_vnode[i] == mount_vnode)
    {
      mountpoints_vnode[i] = 0;
    }
  }
  vfs_free_vnode(mount_vnode);
  return;
}

struct vfs_file_struct * vfs_open(const char * pathname, int flags)
{
  char file_dir_name[0x40];
  struct vfs_vnode_struct * file_vnode = vfs_traverse(pathname, VFS_TRAVERSE_NO_RETURN_NEAREST);
  struct vfs_file_struct * file;

  if(file_vnode == 0)
  {
    if((flags & O_CREAT) == 0)
    {
      return 0;
    }
    /* create file */
    string_copy(pathname, file_dir_name);
    vfs_last_token_in_path(file_dir_name);
    (rootfs -> root -> v_ops -> create)(vfs_traverse(pathname, VFS_TRAVERSE_RETURN_NEAREST), &file_vnode, file_dir_name);
  }

  file = (struct vfs_file_struct *)slab_malloc(sizeof(struct vfs_file_struct));
  file -> vnode = file_vnode;
  file -> read_pos = 0;
  file -> write_pos = 0;
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
  int write_bytes = (file -> f_ops -> write)(file, buf, len);
  file -> write_pos += (unsigned)write_bytes;
  return write_bytes;
}

int vfs_read(struct vfs_file_struct * file, void * buf, size_t len)
{
  /* 1. read min(len, readable file data size) byte to buf from the opened file.
   * 2. return read size or error code if an error occurs.
   */
  int read_bytes = (file -> f_ops -> read)(file, buf, len);
  file -> read_pos += (unsigned)read_bytes;
  return read_bytes;
}

struct vfs_vnode_struct * vfs_traverse(const char * pathname, int return_closest_node)
{
  /* return the closest node if none found if return_closest_node is set*/
  /* otherwise return 0 if none found */

  struct vfs_vnode_struct * search_vnode;
  struct vfs_vnode_struct * target;
  unsigned path_length = (unsigned)string_length(pathname);
  unsigned search_start = 0;

  if(path_length == 1 && pathname[0] == '/')
  {
    (rootfs -> root -> v_ops -> lookup)(rootfs -> root, &target, "/");
    return target;
  }
  if(pathname[0] == '/')
  {
    /* absolute path */
    search_start = 1;
    search_vnode = root_vnode;
  }
  else
  {
    /* relative path */
    search_vnode = single_thread_current_dir_vnode;
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

    if(string_length(component_name) == 1 && component_name[0] == '.')
    {
      /* do nothing */
      continue;
    }
    /* else if(string_length(component_name) == 2 && component_name[1] == '.')
    {
      // Let the actual fs handle it
    }
    */

    /* search componenet in the current directory */
    int lookup_return = (search_vnode -> v_ops -> lookup)(search_vnode, &target, component_name);

    if(target == 0)
    {
      if(return_closest_node)
      {
        return search_vnode;
      }
      else
      {
        vfs_free_vnode(search_vnode);
        return 0;
      }
    }
    else
    {
      if(!lookup_return)
      {
        vfs_free_vnode(search_vnode);
      }
      search_vnode = target;
    }
  }

  return target;
}

int vfs_list(struct vfs_file_struct * file)
{
  (rootfs -> root -> v_ops -> list)(file -> vnode);
  return 0;
}

int vfs_mkdir(struct vfs_vnode_struct * current_dir_vnode, const char * pathname)
{
  char dir_name[0x40];
  string_copy(pathname, dir_name);
  vfs_last_token_in_path(dir_name);
  (current_dir_vnode -> v_ops -> mkdir)(current_dir_vnode, dir_name);
  return 0;
}

int vfs_chdir(struct vfs_vnode_struct * target_dir_vnode)
{
  /* vfs_free_vnode will ignore vnode return by task_get_current_vnode() */
  struct vfs_vnode_struct * current_dir_vnode = single_thread_current_dir_vnode;
  single_thread_current_dir_vnode = target_dir_vnode;
  vfs_free_vnode(current_dir_vnode);
  return 0;
}

struct vfs_filesystem_struct * vfs_get_fs(const char * name)
{
  for(unsigned idx = 0; idx < VFS_MAX_REGISTERED_FS; ++idx)
  {
    if(fs_list[idx] == 0)
    {
      return 0;
    }
    if(string_cmp(fs_list[idx] -> name, name, 999) != 0)
    {
      return fs_list[idx];
    }
  }
  return 0;
}

struct vfs_vnode_struct * vfs_get_root_vnode(void)
{
  return root_vnode;
}

void vfs_last_token_in_path(char * string)
{
  int length = string_length(string);
  int last_token_start;
  int write_idx;

  for(last_token_start = length - 1; last_token_start >= 0 && string[last_token_start] != '/'; --last_token_start);

  /* the entire string is last token */
  if(last_token_start == -1)
  {
    return;
  }

  ++last_token_start;

  for(write_idx = 0; write_idx + last_token_start < length; ++write_idx)
  {
    string[write_idx] = string[write_idx + last_token_start];
  }
  string[write_idx] = '\0';
  return;
}

void vfs_free_vnode(struct vfs_vnode_struct * vnode)
{
  for(int i = 0; i < VFS_MAX_MOUNT; ++i)
  {
    if(mountpoints_vnode[i] == vnode)
    {
      return;
    }
  }
  if(vnode != root_vnode && vnode != single_thread_current_dir_vnode)
  {
    slab_malloc_free((uint64_t *)vnode);
  }
  return;
}

