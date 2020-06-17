#include "kernel/base.h"
#include "kernel/mm.h"
#include "kernel/tmpfs.h"
#include "kernel/vfs.h"
#include "kernel/lib/string.h"

struct filesystem tmpfs = {
  .name = "tmpfs",
  .setup_mount = tmpfs_setup_mount
};

struct file_operations tmpfs_file_operations = {
  .read = tmpfs_read,
  .write = tmpfs_write,
  .readdir = tmpfs_readdir
};

struct vnode_operations tmpfs_vnode_operations = {
  .lookup = tmpfs_lookup,
  .create = tmpfs_create
};

int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount) {
  struct vnode *vn = (struct vnode *)PFN_TO_KVIRT(buddy_alloc(0)->pfn);
  vn->mount = mount;
  vn->f_ops = &tmpfs_file_operations;
  vn->v_ops = &tmpfs_vnode_operations;
  vn->type = VNODE_TYPE_DIR;
  struct tmpfs_dir *dir = (struct tmpfs_dir *)PFN_TO_KVIRT(buddy_alloc(0)->pfn);
  dir->file_nums = 0;
  dir->vn = vn;
  vn->internal = dir;

  mount->fs = fs;
  mount->root = vn;
}

int tmpfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name) {
  struct tmpfs_dir *dir = dir_node->internal;
  for (size_t i = 0; i < dir->file_nums; ++i) {
    if (!strcmp(component_name, dir->files[i].name)) {
      *target = dir->files[i].vn;
      return 0;
    }
  }
  *target = NULL;
  return -1;
}

int tmpfs_create(struct vnode* dir_node, struct vnode** target, const char* component_name) {
  struct tmpfs_dir *dir = dir_node->internal;
  if (dir->file_nums < MAX_FILE_PER_DIR) {
    struct tmpfs_file *file = &dir->files[dir->file_nums++];
    memset(file->contents, 0, MAX_FILE_SIZE);
    strcpy(file->name, component_name);
    file->size = 0;

    struct vnode *vn = (struct vnode *)PFN_TO_KVIRT(buddy_alloc(0)->pfn);
    file->vn = vn;
    vn->f_ops = dir_node->f_ops;
    vn->internal = file;
    vn->mount = dir_node->mount;
    vn->type = VNODE_TYPE_FILE;
    vn->v_ops = dir_node->v_ops;
    *target = vn;
    return 0;
  } else {
    *target = NULL;
    return -1;
  }
}

int tmpfs_read(struct file* file, void* buf, size_t len) {
  struct tmpfs_file *f = (struct tmpfs_file *)file->vnode->internal;
  int nread = 0, pos = file->f_pos;

  if (pos >= f->size) {
    return -1;
  }
  for (; pos < f->size && nread < len; ++pos, ++nread) {
    ((uint8_t *)buf)[pos] = f->contents[pos];
  }

  file->f_pos = pos;
  return nread;
}

int tmpfs_write(struct file* file, const void* buf, size_t len) {
  struct tmpfs_file *f = (struct tmpfs_file *)file->vnode->internal;
  int nwritten = 0, pos = file->f_pos;

  for (; nwritten < len; ++pos, ++nwritten) {
    f->contents[pos] = ((uint8_t *)buf)[pos];
  }
  if (pos > f->size) {
    f->size = pos;
  }

  file->f_pos = pos;
  return nwritten;
}

int tmpfs_readdir(struct file* dir, char* buf, size_t len) {
  if (dir->vnode->type != VNODE_TYPE_DIR) {
    return -1;
  }

  struct tmpfs_dir *d = (struct tmpfs_dir *)dir->vnode->internal;
  if (dir->d_pos >= d->file_nums) {
    return -1;
  }
  strcpy(buf, d->files[dir->d_pos].name);
  dir->d_pos += 1;
  return 0;
}
