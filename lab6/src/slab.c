#include "slab.h"
#include "buddy.h"
#include "printf.h"

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
  self->free_length = nEntries;
  self->length = nEntries;
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
  current->next = 0;
}

/* return the first object from the given slab with specific size */
/* if the size is not belong to this slab, then return false */
/* otherwise return the first object & move the free list node */
int slab_alloc(struct Slab *self, unsigned long size, unsigned long *newLoc) {
  /* the slab is not initialize correctly */
  if (self->size != size || self->free_list == 0) {
    println("[Slab] error: pre-requisite of doing slab alloc not correctly");
    return 0;
  }

  /* the location of the new allocated object should return later */
  *newLoc = (unsigned long)self->free_list;

  /* moving to the next free list node */
  self->free_list = self->free_list->next;

  /* update free length */
  self->free_length--;
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
  /* update free length */
  self->free_length++;
  if (self->free_length == self->length) {
    /* update global slab list */
    global_slab_list = global_slab_list->next;
    /* free page */
    Buddy.dealloc(global_bd, bd_phy2n(self->start));
  }
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
  if (!success || slabStart != slabLoc) {
    println("[Slab] slab_meta alloc failed while allocating a slab for meta");
    return 0;
  }

  /* assign the meta into memory */
  struct Slab *newMeta = (struct Slab *)slabLoc;
  *newMeta = meta;
  return newMeta;
}

struct Slab *global_slab_list = 0;
struct Slab *global_slab_meta = 0;


void *__kalloc(unsigned long size) {
  struct Slab *slab = global_slab_list;

  /* record the return object address */
  unsigned long newLoc;

  /* try to allocate from slab list  */
  for (; slab; slab = slab->next) {
    if (slab_alloc(slab, size, &newLoc)) {
      println("[Slab] found a 'object' in one of slab in current slab list w/ size %d bytes", size);
      return (void *)newLoc;
    }
  }

  println("[Slab] cannot find a object in current slab list, continue to create one");
  /* otherwise continue create a slab  */
  /* and allocate a object from it & connet it to the current slab list */
  /* 1. it need to allocate a meta slab  */
  /* 2. if it used up the meta slab, then create one */
  /* 3. construct the slab */

  /* vvvvv */
  /* 1. allocate a object from meta slab => also a slab */
  unsigned long slabLoc;
  int suc = slab_alloc(global_slab_meta, sizeof(struct Slab), &slabLoc);
  /* if failed to allocate one, then it's mean the meta slab is used up */
  /* continue create one (meta slab) by slab_alloc */
  if (!suc) {
    println("[Slab] used up the meta slab, create one");
    /* request a page for new slab */
    struct Pair p = Buddy.alloc(global_bd, 1);
    /* update the global slab meta */
    global_slab_meta = slab_meta_alloc(p.lb);
    /* allocate a object from global slab meta */
    /* because a object in slab meta aslo a "slab" */
    slab_alloc(global_slab_meta, sizeof(struct Slab), &slabLoc);
  }

  println("[Slab] allocated a new slab from meta slab");

  struct Slab *newSlab = (struct Slab *)slabLoc;


  /* later, we request a page for construct this slab */
  struct Pair p = Buddy.alloc(global_bd, 1);
  init_slab(newSlab, p.lb, size);
  println("[Slab] populated the new slab");

  /* connet to the original slabs */
  newSlab->next = global_slab_list;
  global_slab_list = newSlab;
  println("[Slab] add the new slab to slab list");

  println("[Slab] allocate a 'object' w/ size %d bytes from new slab", size);
  /* finally, we have a slab, and request a object of given size, return in newLoc */
  slab_alloc(newSlab, size, &newLoc);
  return (void*)newLoc;
}


void *kalloc(unsigned long size) {
  return __kalloc(size);
}


void kfree(void *ptr) {
  if (!ptr)
    return;

  unsigned long loc = (unsigned long)ptr;
  for (struct Slab *slab = global_slab_list; slab; slab = slab->next) {
    if (slab_free(slab, loc)) {
      return;
    }
  }
}

/* setup the basic information for future use */
void kalloc_init(struct buddy *bd) {
  /* get the free page from buddy system */
  struct Pair p = Buddy.alloc(bd, 1);
  /* initialize the slab meta */
  global_slab_meta = slab_meta_alloc(p.lb);
}
