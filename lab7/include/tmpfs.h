#pragma once

#include "stddef.h"
#include "vfs.h"

struct __tmpfs {
  struct __tmpfs_type *self;

  unsigned long start;
  unsigned long end;
  unsigned long base;
  unsigned long count;
};


struct __tmpfs_type {
  void (*construct)(struct __tmpfs *self);
  struct __tmpfs *(*new)(unsigned long start, unsigned long end);

  /* only allowed 512byte allocated currently  & without dealloc */
  unsigned long (*alloc)(struct __tmpfs *self);
};


extern struct __tmpfs_type TmpFs;
extern struct __tmpfs *global_tmpfs;

struct filesystem *create_tmpfs();
struct file *tmpfs_create_file(const char *pathname, struct vnode *target);
