#include "tmpfs.h"
#include "libc.h"
#include "mm.h"
#include "slab.h"
#include "vfs.h"

#define BLOCK_SIZE 512

/* using gcc builtin memcopy */
#define memcmp __builtin_memcmp

struct __tmpfs *global_tmpfs;

unsigned long fs_base = 0;

int tmpfs_setup_mount(struct filesystem *fs, struct mount *mount) {}

void init_tmpfs(struct filesystem *tmpfs) {
  tmpfs->name = "tmpfs";
  tmpfs->setup_mount = tmpfs_setup_mount;
}

void tmpfs_construct(struct __tmpfs *self) {} /* not used in this moment */

struct __tmpfs *tmpfs_new(unsigned long start, unsigned long end) {
  struct __tmpfs *new_fs = (struct __tmpfs *)start;
  fs_base = start;
  fs_base += sizeof(struct __tmpfs);

  /* rest of init TODO */
  new_fs->start = fs_base;
  new_fs->end = end;
  new_fs->base = fs_base;
  new_fs->count = 0;

  uart_println("[fs] new tmpfs from %x to %x", new_fs->start, new_fs->end);

  return new_fs;
}

unsigned long tmpfs_alloc(struct __tmpfs *self) {
  /* each file is 512 Byte */
  uart_println("self->base + BLOCKSIZE : %x and self->end %x",
               (self->base + BLOCK_SIZE), self->end);
  if ((self->base + BLOCK_SIZE) < self->end) {
    self->base += BLOCK_SIZE;
    uart_println("[fs] allocated a size of %d byte by tmpfs", BLOCK_SIZE);
    self->count = self->count + 1;
    return self->count - 1;
  }
  uart_println("[fs] fails to allocate a size of %d byte by tmpfs", BLOCK_SIZE);

  return -1;
}

struct __tmpfs_type TmpFs = {
    .construct = tmpfs_construct,
    .new = tmpfs_new,
    .alloc = tmpfs_alloc,
};

struct tmpfs_node {
  const char *name;
  unsigned long blockid;
};

/* lookup the component_name from dir_node and modify the found vnode in target
 */
int tmpfs_node_lookup(struct vnode *dir_node, struct vnode **target,
                      const char *component_name) {
  /* for simplify, current only support single layer */

  struct vnode *list = dir_node->sub_dir;
  while (list) {
    /* found a vnode */
    if ((sstrcmp(list->basename, component_name) == 0)) {
      *target = list;
      return 0;
    }
    list = list->next;
  }

  return -1;
}

unsigned long bid2addr(unsigned long block_id) {
  return BLOCK_SIZE * block_id + global_tmpfs->start;
}

/* TODO */
int tmpfs_file_write(struct file *file, const void *buf, size_t len) {
  unsigned long bid = ((struct tmpfs_node *)(file->vnode->internal))->blockid;
  /* compute address by giving block id */
  uart_println("[fs] write to file w/ block id %d", bid);

  unsigned long addr = bid2addr(bid) + file->f_pos;

  uart_println("  @ %x", addr);

  len = len + file->f_pos > BLOCK_SIZE ? BLOCK_SIZE - file->f_pos : len;
  memcpy((void *)addr, buf, len);

  uart_println("  wirte %d bytes", len);

  /* udpate f_pos  & size */
  file->f_pos += len;
  file->vnode->size =
      file->f_pos > file->vnode->size ? file->f_pos : file->vnode->size;

  uart_println("  current file size: %d byte", file->vnode->size);
  uart_println("  current addr: %x", addr + len);

  return len;
}

int tmpfs_file_read(struct file *file, void *buf, size_t len) {
  unsigned long bid = ((struct tmpfs_node *)(file->vnode->internal))->blockid;

  uart_println("[fs] read from block id %d", bid);

  unsigned long addr = bid2addr(bid) + file->f_pos;

  unsigned long max_addr =
      file->f_pos + len > BLOCK_SIZE ? bid2addr(bid) + BLOCK_SIZE : addr + len;

  uart_println("  read from %x to %x", addr, max_addr);


  int size = 0;
  while (addr < max_addr) {
    if (*(char*)(addr) == 0)
      break;
    size++;
    *(char *)(buf++) = *(char *)(addr++);
  }

  file->f_pos += size;

  /* file->f_pos+=size; */
  /* return size; */

  /* int size = 0; */

  /* while (addr < max_addr && *(unsigned long *)(addr) != 0) { */
  /*   /\* uart_println("meet: %c", *(unsigned long*)(addr)); *\/ */
  /*   size++; */
  /*   *(unsigned long *)buf++ = *(unsigned long *)(addr++); */
  /* } */

  /* /\* udpate f_pos *\/ */
  /* file->f_pos += size; */

  /* return size; */
  return size;
}

int unique_file_id() {
  static int i = 0;
  return i++;
}

struct file *tmpfs_create_file(const char *pathname, struct vnode *target) {
  struct file *file = kalloc(sizeof(struct file));
  file->name = pathname;
  file->f_pos = 0;
  file->f_id = unique_file_id();
  file->vnode = target;
  file->vnode->count++;
  file->f_ops = kalloc(sizeof(struct file_operations));
  file->f_ops->write = tmpfs_file_write;
  file->f_ops->read = tmpfs_file_read;
  return file;
}

int tmpfs_node_create(struct vnode *dir_node, struct vnode **target,
                      const char *component_name) {

  /* otherwise create a vnode */
  struct vnode *new_vnode = kalloc(sizeof(struct vnode));
  new_vnode->internal = kalloc(sizeof(struct tmpfs_node));
  struct tmpfs_node *internal = (struct tmpfs_node *)(new_vnode->internal);
  internal->name = component_name;

  /* create a block */
  unsigned long b = TmpFs.alloc(global_tmpfs);
  uart_println("------> block id : %d", b);
  internal->blockid = b;

  /* create node */
  new_vnode->basename = component_name;
  new_vnode->mount = dir_node->mount;
  new_vnode->v_ops = kalloc(sizeof(struct vnode_operations));
  new_vnode->v_ops->create = tmpfs_node_create;
  new_vnode->v_ops->lookup = tmpfs_node_lookup;
  new_vnode->v_ops->create_file = tmpfs_create_file;
  new_vnode->count = 0;
  new_vnode->parent = 0;
  /* return value */
  *target = new_vnode;

  new_vnode->next = dir_node->sub_dir;
  dir_node->sub_dir = new_vnode;

  return 0;
}

int setup_mount(struct filesystem *fs, struct mount *mount) {
  /* new the tmpfs system */
  unsigned long fs_start = 0x100000;
  unsigned long fs_end = LOW_MEMORY - 1024;
  struct __tmpfs *tmpfs = TmpFs.new((unsigned long)fs_start, fs_end);

  uart_println("fs start %x -> %x", fs_start, fs_end);
  /* memzero(fs_start, fs_end-fs_start); */

  /* return 0; */

  global_tmpfs = tmpfs;
  if (!tmpfs) {
    uart_println("error while constructing the tmpfs system");
    return -1;
  }

  /* create vnode */
  struct vnode *v = kalloc(sizeof(struct vnode));
  v->mount = mount;
  v->internal = kalloc(sizeof(struct tmpfs_node));

  v->v_ops = kalloc(sizeof(struct vnode_operations));

  /* vnode implementation */
  v->v_ops->lookup = tmpfs_node_lookup;
  v->v_ops->create = tmpfs_node_create;
  v->v_ops->create_file = tmpfs_create_file;
  v->count = 1;
  v->next = 0;
  v->sub_dir = 0;
  v->parent = v;
  v->is_dir = 1;
  v->basename = "/";

  /* TODO File operation */

  struct tmpfs_node *internal = (struct tmpfs_node *)(v->internal);
  internal->name = "/";
  uart_println("[fs] created a root vnode @ %s", internal->name);

  /* setup mount */
  mount->fs = fs;
  mount->root = v;
  cur_root = v;

  return 0;
}

/* external use */
struct filesystem *create_tmpfs() {
  struct filesystem *fs = kalloc(sizeof(struct filesystem));
  fs->name = "tmpfs";
  fs->setup_mount = setup_mount;
  return fs;
}
