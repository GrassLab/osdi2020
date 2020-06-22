#ifndef _VFS_H
#define _VFS_H

#include "string.h"
#include "list.h"

#define TMP_FILE_SIZE 512
#define REGULAR_FILE 0 
#define DIRECTORY 1
#define ROOT_DIR 2

#define O_CREAT 1

#define DNAME_LEN 32
#define MAX_CHILD 8

struct dentry {
           /* Pointer to the inode this entry refers to. */
           int flag;
	   char dname[DNAME_LEN];
           struct vnode* vnode;
           int child_count;
           struct dentry *parent_dentry;
	   struct dentry *child_dentry[MAX_CHILD];
	   // number for the register table
	   int is_mount;
}dentry;   


struct vnode {
  struct mount* mount;
  struct vnode_operations* v_ops;
  struct file_operations* f_ops;
  void* internal;
};

struct file {
  struct vnode* vnode;
  size_t f_pos; // The next read/write position of this file descriptor
  struct file_operations* f_ops;
  int flags;
};
	
#define MOUNT_TABLE_SIZE 8
struct mount {
  struct vnode* root;
  struct dentry* dentry;
  struct filesystem* fs;

};

struct filesystem {
  const char* name; 
  int (*setup_mount)(struct filesystem* fs, struct mount* mount);
};

struct file_operations {
  int (*write) (struct file* file, const void* buf, size_t len);
  int (*read) (struct file* file, void* buf, size_t len);
};

struct vnode_operations {
  void (*ls)(struct dentry* dir);
  int (*lookup)(struct dentry* dir, struct vnode** target, const char* component_name);
  int (*create)(struct dentry* dir, struct vnode** target, const char* component_name);
  int (*mkdir)(struct dentry* dir, struct vnode** target, const char *component_name);
};

struct mount* rootfs;
struct dentry* current_dent;

// table for the mounting file system
char mt_table_map[MOUNT_TABLE_SIZE];
struct mount* mount_fs_table[MOUNT_TABLE_SIZE];

void set_dentry(struct dentry *dentry,struct vnode* vnode,const char* str);
void rootfs_init();
int register_filesystem(struct filesystem* fs);
struct file* vfs_open(const char* pathname, int flags);
int vfs_close(struct file* file);
int vfs_write(struct file* file, const void* buf, size_t len);
int vfs_read(struct file* file, void* buf, size_t len);
int vfs_mkdir(const char* pathname);
int vfs_chdir(const char* pathname);
int vfs_mount(const char* device, const char* mountpoint, const char* filesystem);
int vfs_umount(const char* mountpoint);

#endif
