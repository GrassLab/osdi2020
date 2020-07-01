#pragma once

#include "list.h"

struct buddy {
  struct buddy_type *self;
  /* private */
  int size;
  int len;
  struct Node **pair_array;
  unsigned long *map;
};



struct buddy_type {
  void (*construct)(struct buddy *self);
  struct buddy *(*new)(int size, unsigned long base);
  void (*show)(struct buddy *self);
  struct Pair (*alloc)(struct buddy *self, int size);
  void (*dealloc)(struct buddy *self, int size);
};

extern struct buddy_type Buddy;

extern struct buddy *global_bd;


int bd_phy2n(unsigned long addr);
