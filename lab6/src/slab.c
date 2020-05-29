#include "slab.h"
#include "buddy.h"

#define KB_4 (1 << 12)

/* self  : slab itself, the slab memory is come from slab metadata */
/* start : point to the free page from buddy system */
/* size  : entry size */
void init_slab(struct Slab *self, unsigned long start, int size) {
  /* init the single slab */
  self->next = 0;
  self->start = start;
  self->size = size;

  /* populate the slab with entries */
  /* how many entries : calculated by the size of entry */
  unsigned long nEntries = (KB_4 / size) - 1;
  /* free list start from self->start */
  /* the start will computed by base from caller */
  self->free_list = (struct SlabEntry *)self->start;

  /* trace the current pointer in freelist */
  struct SlabEntry *current = self->free_list;
  /* population */
  for (unsigned long i = 1; i < nEntries; ++i) {
    current->next = (struct SlabEntry *)(start + (i * size));
    current = current->next;
  }
}

/* return the first object from the given slab with specific size */
/* if the size is not belong to this slab, then return false */
/* otherwise return the first object & move the free list node */
int slab_alloc(struct Slab *self, unsigned long size, unsigned long *newLoc) {
  /* the slab is not initialize correctly */
  if (self->size != size || self->free_list == 0)
    return 0;

  /* the ocation of the new allocated object should return later */
  *newLoc = (unsigned long)self->free_list;

  /* moving to the next free list node */
  self->free_list = self->free_list->next;
  return 1;
}

/* free the object at location loc & updat freelist */
int slab_free(struct Slab *self, unsigned long loc) {
  /* if the deallocated location is out-of-boundary then return false */
  if (loc < self->start || loc > self->start + KB_4)
    return 0;

  /* trunc the object at location loc to the freed entry */
  struct SlabEntry *freedEntry = (struct SlabEntry *)loc;

  /* * Updat freelist */
  /* connect this entry to the freelist */
  freedEntry->next = self->free_list;
  /* update freelist */
  self->free_list = freedEntry;
  return 1;
}

/* allocate the slab metadata for storing slab struct information */
/* slabStart: needed provide from buddy system */
struct Slab *slab_meta_alloc(unsigned long slabStart) {
  /* create a slab metadata list container, it's also a slab */
  struct Slab meta;

  /* initilize this slab with given slabStart */
  init_slab(&meta, slabStart, sizeof(struct Slab));

  /* allocate the object for current meta information */
  unsigned long slabLoc;
  int success = slab_alloc(&meta, sizeof(struct Slab), &slabLoc);

  /* check the success allocation and it will be allocate at the first meta
   * object */
  if (!success || slabStart != slabLoc)
    return 0;

  /* assign the meta into memory */
  struct Slab *newMeta = (struct Slab *)slabLoc;
  *newMeta = meta;
  return newMeta;
}

struct Slab *global_slab_list = 0;
struct Slab *global_slab_meta = 0;

void *kalloc(unsigned long size) {
  struct Slab *slab = global_slab_list;
  unsigned long newLoc;

  /* try to allocate from slab list  */
  for (; slab; slab = slab->next) {
    if (slab_alloc(slab, size, &newLoc)) {
      return (void *)newLoc;
    }
  }

  /* otherwise continue create a slab  */
  /* and allocate it */
  /* 1. it need to check the meta slab exist */
  /* 2. if it used up the meta slab, then create one */
  /* 3. construct the slab */

  /* but first, check the meta slab exist */
  unsigned long slabLoc;
  int suc = slab_alloc(global_slab_meta, sizeof(struct Slab), &slabLoc);
  /* otherwise create one (meta slab) */
  if (!suc) {
    struct Pair p = Buddy.alloc(global_bd, 1);
    global_slab_meta = slab_meta_alloc(p.lb);
    slab_alloc(global_slab_meta, sizeof(struct Slab), &slabLoc);
  }

  /* construct the slab  */
  struct Slab *newSlab = (struct Slab *)slabLoc;
  struct Pair p = Buddy.alloc(global_bd, 1);
  init_slab(newSlab, p.lb, size);

  /* connet to the original slabs */
  newSlab->next = global_slab_list;
  global_slab_list = newSlab;

  slab_alloc(newSlab, size, &newLoc);
  return (void*)newLoc;
}

unsigned long global_mem_start;

/* setup the basic information for future use */
void kalloc_init(unsigned long mem_start, unsigned long mem_size) {}
