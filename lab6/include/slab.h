#pragma once

struct SlabEntry {
  struct SlabEntry *next;
};

struct Slab {
  struct Slab *next;
  struct SlabEntry *free_list;

  unsigned long start;
  int size;
};

void init_slab(struct Slab *self, unsigned long start, int size);
