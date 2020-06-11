#include "tmpfs.h"
#include "slab.h"
#include "meta_macro.h"
#include "string_util.h"
#include "uart.h"

static struct vfs_vnode_operations_struct * tmpfs_vnode_ops;
static struct vfs_file_operations_struct * tmpfs_file_ops;

void tmpfs_demo_test(void)
{
  char buf[0x20];
  struct vfs_file_struct * a = vfs_open("/hello", O_CREAT);
  struct vfs_file_struct * b = vfs_open("/world", O_CREAT);
  vfs_write(a, "Hello ", 6);
  vfs_write(b, "World!", 6);
  vfs_close(a);
  vfs_close(b);
  b = vfs_open("/hello", 0);
  a = vfs_open("/world", 0);
  int sz;
  sz = vfs_read(b, buf, 100);
  sz += vfs_read(a, buf + sz, 100);
  buf[sz] = '\0';
  uart_puts(buf);
  uart_putc('\n');
  while(1);
}

struct vfs_filesystem_struct * tmpfs_init(void)
{
  /* should be called only once */
  struct vfs_filesystem_struct * fs = (struct vfs_filesystem_struct *)slab_malloc(sizeof(struct vfs_filesystem_struct));

  fs -> name = "tmpfs";
  fs -> setup_mount = tmpfs_mount;

  tmpfs_vnode_ops = (struct vfs_vnode_operations_struct *)slab_malloc(sizeof(struct vfs_vnode_operations_struct));
  tmpfs_file_ops = (struct vfs_file_operations_struct *)slab_malloc(sizeof(struct vfs_file_operations_struct));

  tmpfs_vnode_ops -> lookup = tmpfs_lookup;
  tmpfs_vnode_ops -> create = tmpfs_create;
  tmpfs_file_ops -> write = tmpfs_write;
  tmpfs_file_ops -> read = tmpfs_read;

  vfs_regist_fs(fs);

  return fs;
}

int tmpfs_mount(struct vfs_filesystem_struct * fs, struct vfs_mount_struct * mount)
{
  UNUSED(fs);

  /* create root directory */
  struct vfs_vnode_struct * root_dir_vnode = (struct vfs_vnode_struct *)slab_malloc(sizeof(struct vfs_vnode_struct));

  root_dir_vnode -> mount = mount;
  root_dir_vnode -> internal = (void *)tmpfs_create_dir_node("/");
  root_dir_vnode -> v_ops = tmpfs_vnode_ops;
  root_dir_vnode -> f_ops = tmpfs_file_ops;
  mount -> root = root_dir_vnode;
  return 0;
}

int tmpfs_lookup(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name)
{
  /* TODO: Support for multi level search */
  for(int idx = 0; idx < TMPFS_MAX_FILE_IN_DIR; ++idx)
  {
    if(((((struct tmpfs_dir_node *)(dir_node -> internal)) -> files)[idx]) == 0)
    {
      *target = 0;
      break;
    }
    if(string_cmp(component_name, ((((struct tmpfs_dir_node *)(dir_node -> internal)) -> files)[idx]) -> name, 999))
    {
      struct vfs_vnode_struct * target_vnode = (struct vfs_vnode_struct *)slab_malloc(sizeof(struct vfs_vnode_struct));
      target_vnode -> mount = dir_node -> mount;
      target_vnode -> v_ops = tmpfs_vnode_ops;
      target_vnode -> f_ops = tmpfs_file_ops;
      target_vnode -> internal = (((struct tmpfs_dir_node *)(dir_node -> internal)) -> files)[idx];
      *target = target_vnode;
      break;
    }
  }
  return 0;
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
    struct vfs_vnode_struct * target_vnode = (struct vfs_vnode_struct *)slab_malloc(sizeof(struct vfs_vnode_struct));
    struct tmpfs_file_node * file_node = (struct tmpfs_file_node *)slab_malloc(sizeof(struct tmpfs_file_node));

    memcopy(component_name, file_node -> name, (unsigned)string_length(component_name) + 1);
    file_node -> file_size = 0;
    file_node -> location = 0;

    target_vnode -> mount = dir_node -> mount;
    target_vnode -> v_ops = tmpfs_vnode_ops;
    target_vnode -> f_ops = tmpfs_file_ops;
    target_vnode -> internal = file_node;

    (((struct tmpfs_dir_node *)(dir_node -> internal)) -> files)[idx] = file_node;

    *target = target_vnode;
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

  file_node -> file_size += len;

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
    read_length = (file_node -> file_size - (signed)read_offset);
  }
  else
  {
    read_length = (int)len;
  }
  memcopy((const char *)(location + read_offset), (char *)(buf), (unsigned)read_length);
  return read_length;
}

struct tmpfs_dir_node * tmpfs_create_dir_node(char * dir_name)
{
  struct tmpfs_dir_node * dir_node = (struct tmpfs_dir_node *)slab_malloc(sizeof(struct tmpfs_dir_node));

  dir_node -> name = dir_name;

  /* no subdir and files when created */
  for(unsigned idx = 0; idx < TMPFS_MAX_SUB_DIR; ++idx)
  {
    (dir_node -> subdir_node)[idx] = 0;
  }
  for(unsigned idx = 0; idx < TMPFS_MAX_FILE_IN_DIR; ++idx)
  {
    (dir_node -> files)[idx] = 0;
  }

  return dir_node;
}

