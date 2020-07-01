#include "tmpfs.h"
#include "vfs.h"

struct mount* rootfs;
struct filesystem tmpfs;
int m_token, vo_token, fo_token, f_token;

void init_rootfs() {
  m_token = register_obj_allocator(sizeof(struct mount), FIXED);
  vo_token = register_obj_allocator(sizeof(struct vnode_operations), FIXED);
  fo_token = register_obj_allocator(sizeof(struct file_operations), FIXED);
  f_token = register_obj_allocator(sizeof(struct file), FIXED);

  rootfs = (struct mount*)fixed_obj_allocate(m_token);

  tmpfs.name = "tmpfs";
  tmpfs.setup_mount = tmpfs_mount;
  register_filesystem(&tmpfs);

  tmpfs.setup_mount(&tmpfs, rootfs);
}

int register_filesystem(struct filesystem* fs) {
  // register the file system to the kernel.
  // you can also initialize memory pool of the file system here.
  if (!strcmp(fs->name, "tmpfs")) {
    fs->setup_mount = tmpfs_mount;
    tmpfs_v_ops = (struct vnode_operations*)fixed_obj_allocate(vo_token);
    tmpfs_f_ops = (struct file_operations*)fixed_obj_allocate(fo_token);
    tmpfs_v_ops->lookup = tmpfs_lookup;
    tmpfs_v_ops->create = tmpfs_create;
    tmpfs_f_ops->write = tmpfs_write;
    tmpfs_f_ops->read = tmpfs_read;
    return 1;
  }
  return -1;
}

struct file* vfs_open(const char* pathname, int flags) {
  // 1. Lookup pathname from the root vnode.
  // 2. Create a new file descriptor for this vnode if found.
  // 3. Create a new file if O_CREAT is specified in flags.
  struct vnode* target;
  struct file* fd = 0;
  int lookup = rootfs->root->v_ops->lookup(rootfs->root, &target, pathname);
  if (flags == O_CREAT) {
    if (lookup == -1) {
      rootfs->root->v_ops->create(rootfs->root, &target, pathname);
      fd = (struct file*)fixed_obj_allocate(f_token);
      fd->vnode = target;
      fd->f_ops = target->f_ops;
      fd->f_pos = 0;
    } else {
        uart_puts("File exist!!\n");
    }
  } else {
    if (lookup == -1) {
      uart_puts("File not found!!\n");
    } else {
      fd = (struct file*)fixed_obj_allocate(f_token);
      fd->vnode = target;
      fd->f_ops = target->f_ops;
      fd->f_pos = 0;
    }
  }
  return fd;
}

int vfs_close(struct file* file) {
  uart_puts("Close file Descriptor!!\n");
  fixed_obj_free(file, f_token);
  return 1;
}

int vfs_write(struct file* file, const void* buf, unsigned long len) {
  // 1. write len byte from buf to the opened file.
  // 2. return written size or error code if an error occurs.
  return file->f_ops->write(file, buf, len);
}

int vfs_read(struct file* file, void* buf, unsigned long len) {
  // 1. read min(len, readable file data size) byte to buf from the opened
  // file.
  // 2. return read size or error code if an error occurs.
  return file->f_ops->read(file, buf, len);
}

void vfs_ls(const char* component_name) {
  char *empty = "", *name;
  int len = strlen(component_name);
  name = component_name;
  *(name + len - 1) = '\0';
  tmpfs_ls(rootfs->root, name, empty);
}

void vfs_mkdir(const char* component_name) {
  tmpfs_mkdir(rootfs->root, component_name);
}
