#include "fs.h"
#include "io.h"
#include "tmpfs.h"
#include "allocator.h"

#define xp_func(_path, opset, op, ...) \
  *_path == '/' ? rootfs->root->opset->op( \
      rootfs->root, ## __VA_ARGS__, _path) : \
    current_task->pwd->opset->op( \
      current_task->pwd, ## __VA_ARGS__, _path)

struct mount *rootfs = NULL;

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
    struct directory_operations *dops, 
    void *internal){
  struct vnode *node = (struct vnode *)kmalloc(sizeof(struct vnode));
  node->v_ops = vops;
  node->f_ops = fops;
  node->d_ops = dops;
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
  if(target && target->v_ops->typeof(target) != dirent_file) return NULL;
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

DIR *vfs_opendir(const char *pathname){
  DIR *dir = (DIR*)kmalloc(sizeof(DIR));
  if(xp_func(pathname, d_ops, opendir, dir)){
    return dir;
  }
  kfree(dir);
  return NULL;
}

DIR *vfs_opendir_by_node(
    struct vnode *node,
    const char *pathname){
  DIR *dir = (DIR*)kmalloc(sizeof(DIR));
  if(node->d_ops->opendir(node, dir, pathname)){
    return dir;
  }
  kfree(dir);
  return NULL;
}



dirent *vfs_readdir(DIR *dir){
  return dir->dops->readdir(dir);
}

void vfs_closedir(DIR *dir){
  if(dir->path) kfree(dir->path);
  kfree(dir);
}

int vfs_mkdir(const char *path){
  return xp_func(path, d_ops, mkdir);
  //if(*path == '/')
  //  return rootfs->root->d_ops->mkdir(
  //      rootfs->root, path + 1);
  //return current_task->pwd->d_ops->mkdir(
  //    current_task->pwd, path);
}

int vfs_chdir(const char *path){
  return xp_func(path, d_ops, chdir);
}


int vfs_mount(
    const char *dev, const char *mp, const char *fs){
  return 0;
}

int vfs_umount(const char *mp){
  return 0;
}

void fs_init(){
  register_filesystem(tmpfs);
  tmpfs->setup_mount(tmpfs, rootfs = newMnt());
}

void indent(int n){
  for(int i = 0; i < n * 2; i ++){
    printf(" ");
  }
}

void list_dir(DIR *dir, int lv){
  dirent *entry;
  indent(lv);
  printfmt("{%s}", dir->path);
  while((entry = vfs_readdir(dir))){
    if(entry->type == dirent_dir){
      DIR *subd = vfs_opendir_by_node(
          dir->root, entry->name);
      list_dir(subd, lv + 1);
      vfs_closedir(subd);
    }
    else{
      indent(lv + 1);
      printf("<%s>" NEWLINE, entry->name);
    }
  }
}


