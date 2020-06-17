#include "kernel/base.h"
#include "kernel/mm.h"
#include "kernel/lib/string.h"
#include "kernel/vfs.h"

int register_filesystem(struct filesystem* fs) {
  if (filesystem_num < MAX_FILESYSTEM_NUM) {
    filesystems[filesystem_num++] = fs;
  }
}

struct file* vfs_open(const char* pathname, int flags) {
  char path[MAX_PATHNAME_LEN];
  strcpy(path, pathname);
  struct vnode *cur_node = rootfs->root, *prev_node = rootfs->root;
  char *cur_comp = strtok(path, "/ \n"), *prev_comp;

  while (cur_comp != NULL) {
    prev_node = cur_node;
    cur_node->v_ops->lookup(cur_node, &cur_node, cur_comp);
    prev_comp = cur_comp;
    cur_comp = strtok(NULL, "/ \n");
  }

  if (cur_node == NULL) {
    if (flags & O_CREAT) {
      prev_node->v_ops->create(prev_node, &cur_node, prev_comp);
    } else {
      return NULL;
    }
  }

  struct file *f = (struct file *)PFN_TO_KVIRT(buddy_alloc(0)->pfn);;
  f->f_ops = prev_node->f_ops;
  f->f_pos = 0;
  f->flags = flags;
  f->vnode = cur_node;
  return f;
}

int vfs_close(struct file* file) {
  buddy_free(&pages[KVIRT_TO_PFN((uint64_t)file)], 0);
}

int vfs_read(struct file* file, void* buf, size_t len) {
  file->f_ops->read(file, buf, len);
}

int vfs_write(struct file* file, const void* buf, size_t len) {
  file->f_ops->write(file, buf, len);
}

int vfs_readdir(struct file* dir, char* buf, size_t len) {
  dir->f_ops->readdir(dir, buf, len);
}
