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

extern unsigned long global_mem_start;
extern struct Slab* global_slab_meta;
extern struct Slab* global_slab_list;

void *kalloc(unsigned long size);
struct Slab *slab_meta_alloc(unsigned long slabStart);
void init_slab(struct Slab *self, unsigned long start, int size);
