#include "fs.h"
#include "tmpfs.h"
#include "allocator.h"

struct mount *rootfs = NULL;
struct vnode *pwd = NULL;

struct filesystem *regedfs = NULL;

struct mount *newMnt(){
  struct mount *newmnt = (struct mount*)kmalloc(sizeof(struct mount));
  newmnt->root = NULL, newmnt->fs = NULL;
  return newmnt;
}

struct file *newFd(struct vnode *vnode, int flags){
  if(!vnode) return NULL;
  struct file *fd = (struct file*)kmalloc(sizeof(struct file));
  fd->vnode = vnode, fd->flags = flags;
  fd->f_pos = 0, fd->f_ops = fd->vnode->f_ops;
  return fd;
}

struct vnode *newVnode(
    struct mount *mount,
    struct vnode_operations *vops,
    struct file_operations *fops, 
    void *internal){
  struct vnode *node = (struct vnode *)kmalloc(sizeof(struct vnode));
  node->v_ops = vops;
  node->f_ops = fops;
  node->internal = internal;
  return node;
}

int register_filesystem(struct filesystem *fs) {
  // register the file system to the kernel.
  fs->nextfs = regedfs;
  regedfs = fs;
  // you can also initialize memory pool 
  // of the file system here.
  return 0;
}

struct file *vfs_open(const char *pathname, int flags) {
  // 1. Lookup pathname from the root vnode.
  struct vnode *target = 0;
  rootfs->root->v_ops->lookup(rootfs->root, &target, pathname);
  // 2. Create a new file descriptor for this vnode if found.
  if(!target && flags & O_CREAT)
    rootfs->root->v_ops->create(rootfs->root, &target, pathname);
  // 3. Create a new file if O_CREAT is specified in flags.
  return newFd(target, flags);
}

int vfs_close(struct file *file) {
  // 1. release the file descriptor
  kfree(file);
  return 0;
}

int vfs_write(struct file *file, const void *buf, size_t len) {
  // 1. write len byte from buf to the opened file.
  return file->vnode->f_ops->write(file, buf, len);
  // 2. return written size or error code if an error occurs.
}

int vfs_read(struct file *file, void *buf, size_t len) {
  // 1. read min(len, readable file data size) byte to buf from the opened file.
  return file->vnode->f_ops->read(file, buf, len);
  // 2. return read size or error code if an error occurs.
  return 0;
}

void vfs_show(){
  tmpfs_show_tree(rootfs->root, 0);
}

//void vfs_readdir(struct file *file){
//  file->vnode->
//  tmpfs_show_tree(rootfs->root, 0);
//}

void fs_init(){
  register_filesystem(tmpfs);
  tmpfs->setup_mount(tmpfs, rootfs = newMnt());
}
