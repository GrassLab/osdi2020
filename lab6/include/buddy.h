#pragma once

#include "list.h"

struct buddy {
  struct buddy_type *self;
  /* private */
  int size;
  int len;
  struct Node **pair_array;
};

/* struct Pair extend_4kb(struct Pair p) { */
/*   return (struct Pair){p.lb << 12, p.ub << 12}; */
/* } */

struct buddy_type {
  void (*construct)(struct buddy *self);
  struct buddy *(*new)(int size, unsigned long base);
  void (*show)(struct buddy *self);
  struct Pair (*alloc)(struct buddy *self, int size);
};

extern struct buddy_type Buddy;
