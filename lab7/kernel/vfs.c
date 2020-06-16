#include "vfs.h"
#include "libc.h"
#include "slab.h"
#include "tmpfs.h"

struct mount *rootfs;
struct vnode *cur_root;

/* path name solver */
struct path {
  const char *name;
  struct path *next;
};
/* only construct the pathname list, it will not check the vnode exist or not */
struct path *path_name_solver(const char *pathname) {

}


void show_dir(struct vnode *v) {
  /* show the directory information */
  struct vnode *list = v->sub_dir;
  while (list) {
    if (list->is_dir) {
      uart_print("%s/  ", list->basename);
    } else {
      uart_print("%s  ", list->basename);
    }

    list = list->next;
  }
  uart_println("");
}

struct file *vfs_open(const char *pathname, int flags) {
  // 1. Lookup pathname from the root vnode.
  // 2. Create a new file descriptor for this vnode if found.
  // 3. Create a new file if O_CREAT is specified in flags.

  /* lookup pathname from root vnode */
  struct vnode *root = cur_root;
  struct vnode *target;

  /* file descriptor */
  /* lookup */
  if (root->v_ops->lookup(root, &target, pathname) == 0) {
    /* create a new file descriptor */
    /* TODO should eliminate the dependence with tmpfs */
    /* root->f_ops->write */
    struct file *file = root->v_ops->create_file(pathname, target);
    /* struct file *file = tmpfs_create_file(pathname, target); */

    uart_println("[fs] created a file descriptor : %s", target->basename);
    uart_println("  node count: %d", file->vnode->count);
    uart_println("  file id: %d", file->f_id);
    uart_println("  mount @ %s", target->mount->fs->name);

    return file;
  }

  /* create a node & file will point to the new node */
  if (flags == O_CREAT && root->v_ops->create(root, &target, pathname) == 0) {
    /* create a new file descriptor */
    /* TODO should eliminate the dependence with file */
    struct file *file = root->v_ops->create_file(pathname, target);

    uart_println("[fs] created a file descriptor & vnode : %s",
                 target->basename);
    uart_println("  node count: %d to %d", file->vnode->count - 1,
                 file->vnode->count);
    uart_println("  file id: %d", file->f_id);
    uart_println("  mount @ %s", target->mount->fs->name);
    return file;
  }

  if (sstrcmp(pathname, ".") == 0) {
    struct file *root_file = tmpfs_create_file(root->basename, root);
    show_dir(root);
    return root_file;

  } else if (sstrcmp(pathname, root->basename) == 0) {
    struct file *root_file = tmpfs_create_file("/", root);
    uart_println("[fs] created a file descriptor : %s", root->basename);
    uart_println("  node count: %d", root_file->vnode->count);
    uart_println("  file id: %d", root_file->f_id);
    uart_println("  mount @ %s", root->mount->fs->name);

    show_dir(root);

    return root_file;
  }

  return 0;
}
int vfs_close(struct file *file) {
  // 1. release the file descriptor
  uart_println("[fs] close a file %s, count = %d to %d", file->name,
               file->vnode->count, file->vnode->count - 1);
  if (--file->vnode->count == 0) {
    /* release file */
    kfree(file);
  }
  return 0;
}

int vfs_write(struct file *file, const void *buf, size_t len) {
  // 1. write len byte from buf to the opened file.
  // 2. return written size or error code if an error occurs.
  return file->f_ops->write(file, buf, len);
}
int vfs_read(struct file *file, void *buf, size_t len) {
  // 1. read min(len, readable file data size) byte to buf from the opened file.
  // 2. return read size or error code if an error occurs.

  return file->f_ops->read(file, buf, len);
}

int vfs_mkdir(const char *pathname) {
  struct file *file = vfs_open(pathname, O_CREAT);
  if (file) {
    struct vnode *v = file->vnode;
    v->is_dir = 1;
    v->parent = cur_root;
    vfs_close(file);
    return 0;
  }
  return -1;
}

int vfs_chdir(const char *pathname) {

  int found = -1;

  if ((sstrcmp(pathname, "..")) == 0) {
    cur_root = cur_root->parent;
    return 0;
  }

  struct vnode *list = cur_root->sub_dir;
  while (list) {
    if (sstrcmp(pathname, list->basename) == 0 && list->is_dir) {
      /* found */
      found = 0;
      cur_root = list;
      break;
    }
    list = list->next;
  }

  return found;
}

int vfs_mount(const char *device, const char *mountpoint,
              const char *filesystem) {}

int vfs_umount(const char *mountpoint);
