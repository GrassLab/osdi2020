#include "vfs.h"
#include "libc.h"
#include "slab.h"
#include "tmpfs.h"

struct mount *rootfs;
struct vnode *cur_root;

size_t strlen(const char *str) {
  const char *s;

  for (s = str; *s; ++s)
    ;
  return (s - str);
}

int currIndex = 0;
char *my_strtok(const char *s, char *delm) {
  if (!s || !delm || s[currIndex] == '\0')
    return NULL;
  char *W = kalloc(sizeof(char) * 100);
  memset(W, 0, sizeof(char) * 100);
  int i = currIndex, k = 0, j = 0;

  while (s[i] != '\0') {
    j = 0;
    while (delm[j] != '\0') {
      if (s[i] != delm[j])
        W[k] = s[i];
      else
        goto It;
      j++;
    }

    i++;
    k++;
  }
It:
  W[i] = 0;
  currIndex = i + 1;
  // Iterator = ++ptr;
  return W;
}

/* only construct the pathname list, it will not check the vnode exist or not */
struct path *path_name_solver(const char *pathname) {
  struct path *head = kalloc(sizeof(struct path));
  struct path *ret = head;
  head->name = 0;
  uart_println("======== start path solver =========");

  char delm[] = "/";
  if (*pathname == '/') {
    uart_println("found / & store to path");
    head->name = "/";
    head->next = kalloc(sizeof(struct path));
    head = head->next;
    head->name = 0;
    my_strtok(pathname, delm);
  }

  char *str = my_strtok(pathname, delm);
  while (str) {
    uart_println("found %s & store to path", str);
    head->name = str;
    head->next = kalloc(sizeof(struct path));
    head = head->next;
    head->name = 0;
    /* kfree(str); */
    str = my_strtok(pathname, delm);
  }

  currIndex = 0;

  uart_println("========= end path solver =========");
  return ret;
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

struct file *__vfs_open(const char *pathname, int flags) {
  // 1. Lookup pathname from the root vnode.
  // 2. Create a new file descriptor for this vnode if found.
  // 3. Create a new file if O_CREAT is specified in flags.

  /* lookup pathname from root vnode */
  struct vnode *root = cur_root;
  struct vnode *target;
  if (root->v_ops->lookup(root, &target, pathname) == 0) {

    struct file *fd = kalloc(sizeof(struct file));
    fd->vnode = target;
    fd->flags = flags;
    fd->f_pos = 0;
    fd->f_ops = fd->vnode->f_ops;

    return fd;
  }
  return 0;





  /* file descriptor */
  /* lookup */
  if (root->v_ops->lookup(root, &target, pathname) == 0) {
    /* create a new file descriptor */
    /* root->f_ops->write */
    struct file *file = root->v_ops->create_file(pathname, target);

    /* struct file *file = tmpfs_create_file(pathname, target); */

    uart_println("[fs] created a file descriptor : %s", target->basename);
    uart_println("  node count: %d", file->vnode->count);
    uart_println("  file id: %d", file->f_id);
    uart_println("  mount @ %s", target->mount->fs->name);

    if (file->vnode->is_dir) {
      show_dir(file->vnode);
    }

    return file;
  }

  /* create a node & file will point to the new node */
  if (flags == O_CREAT && root->v_ops->create(root, &target, pathname) == 0) {
    /* create a new file descriptor */
    struct file *file = root->v_ops->create_file(pathname, target);

    uart_println("[fs] created a file descriptor & vnode : %s",
                 target->basename);
    uart_println("  node count: %d to %d", file->vnode->count - 1,
                 file->vnode->count);
    uart_println("  file id: %d", file->f_id);
    uart_println("  mount @ %s", target->mount->fs->name);
    return file;
  }

  if (sstrcmp(pathname, "/") == 0) {
    struct file *root_file =
        root->v_ops->create_file(rootfs->root->basename, rootfs->root);
    show_dir(rootfs->root);
    return root_file;

  } else if (sstrcmp(pathname, ".") == 0) {
    struct file *root_file = root->v_ops->create_file(root->basename, root);
    show_dir(root);
    return root_file;

  } else if (sstrcmp(pathname, root->basename) == 0) {
    struct file *root_file = root->v_ops->create_file("/", root);
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

int __vfs_mkdir(const char *pathname) {
  if ((sstrcmp(pathname, "/")) == 0) {
    return 0;
  }

  struct file *file = __vfs_open(pathname, O_CREAT);

  if (file) {
    struct vnode *v = file->vnode;
    v->is_dir = 1;
    v->parent = cur_root;
    vfs_close(file);
    return 0;
  }
  return -1;
}

int __vfs_chdir(const char *pathname) {
  int found = -1;

  if ((sstrcmp(pathname, "/")) == 0) {
    cur_root = rootfs->root;
    return 0;
  }

  if ((sstrcmp(pathname, "..")) == 0) {
    cur_root = cur_root->parent;
    return 0;
  }

  if ((sstrcmp(pathname, ".")) == 0) {
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

int vfs_mkdir(const char *pathname) {
  struct path *head = path_name_solver(pathname);
  struct vnode *back = cur_root;
  while (head && head->name) {

    /* main */
    if (__vfs_mkdir(head->name) != 0) {
      uart_println("[fs] failed to mkdir %s", head->name);
      cur_root = back;
      return -1;
    }

    __vfs_chdir(head->name);

    head = head->next;
  }
  cur_root = back;
  return 0;
}

int vfs_chdir(const char *pathname) {
  struct path *head = path_name_solver(pathname);
  struct vnode *back = cur_root;
  while (head && head->name) {

    /* main */
    if (__vfs_chdir(head->name) != 0) {
      uart_println("[fs] failed to chdir %s", head->name);
      cur_root = back;
      return -1;
    }

    head = head->next;
  }
  return 0;
}

struct file *vfs_open(const char *pathname, int flags) {
  struct file *file = __vfs_open(pathname, flags);
  return file;

  /* struct path *head = path_name_solver(pathname); */
  /* struct vnode *back = cur_root; */
  /* struct path *end = head; */
  /* while (end && end->next->name) { */
  /*   uart_println("||open|| dir: %s", end->name); */
  /*   end = end->next; */
  /* } */
  /* uart_println("||open|| file or dir: %s", end->name); */

  /* if (head == end) */
  /*   return __vfs_open(pathname, flags); */

  /* while (head != end) { */
  /*   if (__vfs_chdir(head->name) != 0) { */
  /*     return 0; */
  /*   } */
  /*   head = head->next; */
  /* } */

  /* struct file *file = __vfs_open(head->name, flags); */
  /* cur_root = back; */
  /* return file; */
}
int mIndex = 0;
struct MountP {
  const char *point;
  struct vnode *node;
};
struct MountP *mount_points[1024] = {
    0,
};

int vfs_mount(const char *device, const char *mountpoint,
              const char *filesystem) {
  struct file *f = vfs_open(mountpoint, 0);
  if (!f) {
    uart_println("[fs] mount failed");
    return -1;
  }
  struct vnode *v = f->vnode;
  struct MountP *m = kalloc(sizeof(struct MountP));
  struct vnode *n = kalloc(sizeof(struct vnode));

  if ((sstrcmp(device, "tmpfs")) == 0) {
    memcpy(n, v, sizeof(struct vnode));
    /* memcpy(v, rootfs->root, sizeof(struct vnode)); */

    v->sub_dir = rootfs->root->sub_dir;

    uart_println("[fs] mount on %s", v->basename);

    m->point = mountpoint;
    m->node = n;
    mount_points[mIndex++] = m;
    return 0;
  }

  return -1;
}

int vfs_umount(const char *mountpoint) {

  struct file *f = vfs_open(mountpoint, 0);
  if (!f) {
    uart_println("[fs] unmount failed");
    return -1;
  }
  struct vnode *v = f->vnode;
  for (int i = 0; i < 1024; ++i) {
    if (sstrcmp(mount_points[i]->point, mountpoint) == 0) {

      memcpy(v, mount_points[i]->node, sizeof(struct vnode));

      mount_points[i] = 0;
      return 0;
    }
  }

  uart_println("[fs] unmount failed");
  return -1;
}
