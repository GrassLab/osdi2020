#include "tmpfs.h"
#include "slab.h"
#include "meta_macro.h"
#include "string_util.h"
#include "uart.h"

static struct vfs_vnode_operations_struct * tmpfs_vnode_ops;
static struct vfs_file_operations_struct * tmpfs_file_ops;

struct vfs_filesystem_struct * tmpfs_init(void)
{
  /* should be called only once */
  struct vfs_filesystem_struct * fs = (struct vfs_filesystem_struct *)slab_malloc(sizeof(struct vfs_filesystem_struct));

  fs -> name = "tmpfs";
  fs -> setup_mount = tmpfs_setup_mount;

  tmpfs_vnode_ops = (struct vfs_vnode_operations_struct *)slab_malloc(sizeof(struct vfs_vnode_operations_struct));
  tmpfs_file_ops = (struct vfs_file_operations_struct *)slab_malloc(sizeof(struct vfs_file_operations_struct));

  tmpfs_vnode_ops -> lookup = tmpfs_lookup;
  tmpfs_vnode_ops -> create = tmpfs_create;
  tmpfs_vnode_ops -> list = tmpfs_list;
  tmpfs_vnode_ops -> mkdir = tmpfs_mkdir;
  tmpfs_vnode_ops -> mount = tmpfs_mount;
  tmpfs_vnode_ops -> umount = tmpfs_umount;
  tmpfs_file_ops -> write = tmpfs_write;
  tmpfs_file_ops -> read = tmpfs_read;

  vfs_regist_fs(fs);

  return fs;
}

int tmpfs_setup_mount(struct vfs_filesystem_struct * fs, struct vfs_mount_struct * mount)
{
  UNUSED(fs);

  /* create root directory */
  struct vfs_vnode_struct * root_dir_vnode = tmpfs_create_vnode(mount, (void *)tmpfs_create_dir_node("/"), 1);

  ((struct tmpfs_dir_node *)(root_dir_vnode -> internal)) -> parent_node = (struct tmpfs_dir_node *)(root_dir_vnode -> internal);

  mount -> root = root_dir_vnode;

  return 0;
}

int tmpfs_mount(struct vfs_vnode_struct * mountpoint_vnode, struct vfs_mount_struct * mount)
{
  struct tmpfs_dir_node * dir_node = (struct tmpfs_dir_node *)(mountpoint_vnode -> internal);

  /* setup mountoint in parent's list */
  struct tmpfs_dir_node * parent_dir_node = dir_node -> parent_node;

  for(int i = 0; i < TMPFS_MAX_SUB_DIR; ++i)
  {
    if(parent_dir_node -> subdir_node[i] == 0)
    {
      break;
    }
    if(string_cmp(parent_dir_node -> subdir_node[i] -> name, dir_node -> name, 999) != 0)
    {
      parent_dir_node -> mountpoints[i] = mount;
      break;
    }
  }

  ((struct tmpfs_dir_node *)(mount -> root -> internal)) -> parent_node = parent_dir_node;
  return 0;
}

int tmpfs_umount(struct vfs_vnode_struct * mountpoint_parent, const char * mountpoint_token)
{
  struct tmpfs_dir_node * parent_dir_node = (struct tmpfs_dir_node *)(mountpoint_parent -> internal);

  for(int i = 0; i < TMPFS_MAX_SUB_DIR; ++i)
  {
    if(string_cmp(((parent_dir_node -> subdir_node)[i]) -> name, mountpoint_token, 999) != 0)
    {
      (parent_dir_node -> mountpoints)[i] = 0;
      break;
    }
  }
  return 0;
}

int tmpfs_lookup(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name)
{
  /* return 0 if node should be free after use, 1 if not */
  struct tmpfs_dir_node * internal_dir_node = (struct tmpfs_dir_node *)(dir_node -> internal);
  if(component_name[0] == '/')
  {
    *target = tmpfs_create_vnode(dir_node -> mount, dir_node -> internal, 1);
    return 0;
  }
  if(string_length(component_name) == 2 && component_name[0] == '.' && component_name[1] == '.')
  {
    *target = tmpfs_create_vnode(dir_node -> mount, internal_dir_node -> parent_node, 1);
    return 0;
  }

  /* search file */
  for(int idx = 0; idx < TMPFS_MAX_FILE_IN_DIR; ++idx)
  {
    if((internal_dir_node -> files)[idx] == 0)
    {
      *target = 0;
      break;
    }
    if(string_cmp(component_name, ((internal_dir_node -> files)[idx]) -> name, 999))
    {
      *target = tmpfs_create_vnode(dir_node -> mount, (internal_dir_node -> files)[idx], 0);
      return 0;
    }
  }

  /* search directory */
  for(int idx = 0; idx < TMPFS_MAX_SUB_DIR; ++idx)
  {
    if(((internal_dir_node -> subdir_node)[idx]) == 0)
    {
      *target = 0;
      break;
    }
    if(string_cmp(component_name, ((internal_dir_node -> subdir_node)[idx]) -> name, 999))
    {
      /* check mountpoint */
      if(((internal_dir_node -> mountpoints)[idx]) != 0)
      {
        *target = ((internal_dir_node -> mountpoints)[idx]) -> root;
        return 1;
      }

      *target = tmpfs_create_vnode(dir_node -> mount, (internal_dir_node -> subdir_node)[idx], 1);
      return 0;
    }
  }
  return 1;
}

int tmpfs_create(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name)
{
  /* TODO: Support for multi level search */
  for(int idx = 0; idx < TMPFS_MAX_FILE_IN_DIR; ++idx)
  {
    if(((((struct tmpfs_dir_node *)(dir_node -> internal)) -> files)[idx]) != 0)
    {
      continue;
    }
    struct tmpfs_file_node * file_node = (struct tmpfs_file_node *)slab_malloc(sizeof(struct tmpfs_file_node));

    string_copy(component_name, file_node -> name);
    file_node -> file_size = 0;
    file_node -> location = 0;

    (((struct tmpfs_dir_node *)(dir_node -> internal)) -> files)[idx] = file_node;

    *target = tmpfs_create_vnode(dir_node -> mount, (void *)file_node, 0);
    return 0;
  }
  return 0;
}

int tmpfs_write(struct vfs_file_struct * file, const void * buf, size_t len)
{
  struct tmpfs_file_node * file_node = (struct tmpfs_file_node *)(file -> vnode -> internal);
  uint64_t * location = file_node -> location;
  unsigned write_offset = (unsigned)(file -> write_pos);
  if(location == 0)
  {
    location = (uint64_t *)slab_malloc(TMPFS_MAX_FILE_SIZE);
    file_node -> location = location;
  }
  memcopy((const char *)buf, (char *)(location + write_offset), (unsigned)len);

  file_node -> file_size += (unsigned)len;

  return (int)len;
}

int tmpfs_read(struct vfs_file_struct * file, void * buf, size_t len)
{
  struct tmpfs_file_node * file_node = (struct tmpfs_file_node *)(file -> vnode -> internal);
  uint64_t * location = file_node -> location;
  unsigned read_offset = (unsigned)(file -> write_pos);
  int read_length;
  if(len > (file_node -> file_size - read_offset - 1))
  {
    read_length = (signed)(file_node -> file_size - read_offset);
  }
  else
  {
    read_length = (int)len;
  }
  memcopy((const char *)(location + read_offset), (char *)(buf), (unsigned)read_length);
  return read_length;
}

int tmpfs_list(struct vfs_vnode_struct * dir_node)
{
  struct tmpfs_dir_node * internal_dir_node = (struct tmpfs_dir_node *)(dir_node -> internal);
  uart_puts("Files inside ");
  uart_puts(internal_dir_node -> name);
  uart_puts(" :\n");
  for(int i = 0; i < TMPFS_MAX_SUB_DIR; ++i)
  {
    if((internal_dir_node -> subdir_node)[i] == 0)
    {
      break;
    }
    uart_puts("d ");
    uart_puts(((internal_dir_node -> subdir_node)[i]) -> name);
    uart_putc('\n');
  }
  for(int i = 0; i < TMPFS_MAX_FILE_IN_DIR; ++i)
  {
    if((internal_dir_node -> files)[i] == 0)
    {
      break;
    }
    uart_puts("- ");
    uart_puts(((internal_dir_node -> files)[i]) -> name);
    uart_putc('\n');
  }
  return 0;
}

int tmpfs_mkdir(struct vfs_vnode_struct * dir_node, const char * new_dir_name)
{
  struct tmpfs_dir_node * current_dir_node = (struct tmpfs_dir_node *)(dir_node -> internal);
  struct tmpfs_dir_node * new_dir_node = tmpfs_create_dir_node(new_dir_name);

  new_dir_node -> parent_node = current_dir_node;

  for(int i = 0; i < TMPFS_MAX_SUB_DIR; ++i)
  {
    if((current_dir_node -> subdir_node)[i] == 0)
    {
      (current_dir_node -> subdir_node)[i] = new_dir_node;
      break;
    }
  }
  return 0;
}

struct tmpfs_dir_node * tmpfs_create_dir_node(const char * dir_name)
{
  struct tmpfs_dir_node * dir_node = (struct tmpfs_dir_node *)slab_malloc(sizeof(struct tmpfs_dir_node));

  string_copy(dir_name, dir_node -> name);

  /* no subdir, files, mountpoints when created */
  for(unsigned idx = 0; idx < TMPFS_MAX_SUB_DIR; ++idx)
  {
    (dir_node -> subdir_node)[idx] = 0;
    (dir_node -> mountpoints)[idx] = 0;
  }
  for(unsigned idx = 0; idx < TMPFS_MAX_FILE_IN_DIR; ++idx)
  {
    (dir_node -> files)[idx] = 0;
  }

  return dir_node;
}

struct vfs_vnode_struct * tmpfs_create_vnode(struct vfs_mount_struct * mount, void * internal, int is_dir)
{
  struct vfs_vnode_struct * target_vnode = (struct vfs_vnode_struct *)slab_malloc(sizeof(struct vfs_vnode_struct));
  target_vnode -> mount = mount;
  target_vnode -> v_ops = tmpfs_vnode_ops;
  target_vnode -> f_ops = tmpfs_file_ops;
  target_vnode -> internal = internal;
  target_vnode -> is_dir = is_dir;
  return target_vnode;
}

