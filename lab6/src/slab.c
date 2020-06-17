#include "slab.h"
#include "buddy.h"
#include "printf.h"

#define KB_4 (1 << 12)

unsigned long next_pow2(unsigned long x) {
  return x == 1 ? 1 : 1 << (64 - __builtin_clzl(x - 1));
}

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
  println("[Slab:Free] freed the object w/ size %d", self->size);
  if (self->free_length == self->length && self->fixed != 1) {
    println("[Slab:Free] remove the current object allocator w/ size %d",
            self->size);
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

struct Slab *register_allocator(unsigned long size, int fixed) {
  /* 1. it need to allocate a meta slab  */
  /* 2. if it used up the meta slab, then create one */
  /* 3. construct the slab */
  /* 4. connect the new slab into slab list */

  /* vvvvv */
  /* 1. allocate a object from meta slab => also a slab */
  unsigned long slabLoc;
  int suc = slab_alloc(global_slab_meta, sizeof(struct Slab), &slabLoc);
  /* if failed to allocate one, then it's mean the meta slab is used up */
  /* continue create one (meta slab) by slab_alloc */
  if (!suc) {
    /* 2. used up the meta slab, create one */
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

  /* 3. later, we request a page for construct this slab */
  struct Pair p = Buddy.alloc(global_bd, 1);
  init_slab(newSlab, p.lb, size);
  println("[Slab] populated the new slab w/ size: %d bytes", size);

  /* 4. connet to the original slabs list */
  newSlab->next = global_slab_list;
  global_slab_list = newSlab;
  println("[Slab] added the new slab to slab list");

  newSlab->fixed = fixed;
  return newSlab;
}

void *__kalloc(unsigned long size) {
  struct Slab *slab = global_slab_list;

  /* record the return object address */
  unsigned long newLoc;

  /* try to allocate from slab list  */
  for (; slab; slab = slab->next) {
    if (slab_alloc(slab, size, &newLoc)) {
      println("[Slab] found a 'object' in one of slab in current slab list w/ "
              "size %d bytes",
              size);
      return (void *)newLoc;
    }
  }

  println("[Slab] cannot find a object in current slab list, continue to "
          "create one");
  /* otherwise continue create a slab  & connect it to the current slab list*/
  struct Slab *newSlab = register_allocator(size, 0);

  /* finally, we have a slab, and request a object of given size, return in
   * newLoc */
  slab_alloc(newSlab, size, &newLoc);
  println("[Slab] allocated a 'object' w/ size %d bytes from new slab", size);
  return (void *)newLoc;
}

void *kalloc(unsigned long size) {
  /* the request size should not smaller than 0 */
  if (size < 0) {
    println("[Slab] invalid request of size %d", size);
    return 0;
  }
  /* only accept the size is greater than 8Byte */
  /* because a miniumn slab entry is equal to 8Byte */
  /* so, if the request size is smaller than 8Byte */
  /* then allocate the sizeof 8Byte & return */
  if (size < 8) {
    println(
        "[Slab] the request size %d is too small, allocate 8Byte for this item",
        size);
    size = 8;
  }

  /* if the size it greater than 1 page, return false currently */
  if (size > (1 << 12)) {
    println("[Slab] the request size %d is too large, return false");
    return 0;
  }

  /* otherwise continue allocate the object */
  /* try to round up the current size, for finding a allocator to provide a object for it */
  unsigned long new_size = next_pow2(size);
  if (new_size != size) {
    println("[Slab] roundup the size %d to %d", size, new_size);
  }

  /* if the object can be allocated by current allocator, then allocate it,
   * otherwise, create one */
  return __kalloc(new_size);
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

int default_size_list[8] = {8, 16, 32, 64, 96, 128, 192, 256};

/* setup the basic information for future use */
void kalloc_init(struct buddy *bd) {
  /* get the free page from buddy system */
  struct Pair p = Buddy.alloc(bd, 1);

  /* initialize the slab meta */
  println("[Kalloc:Init] construct a meta slab");
  global_slab_meta = slab_meta_alloc(p.lb);

  /* register list of allocator for this */
  /* name(-:notsupport) objsize  obj/slab   */
  /* -kmalloc-8192       8192          4    */
  /* -kmalloc-4096       4096          8    */
  /* -kmalloc-2048       2048         16    */
  /* -kmalloc-1024       1024         16    */
  /* -kmalloc-512         512         16    */
  /* kmalloc-256          256         16    */
  /* kmalloc-192          192         21    */
  /* kmalloc-128          128         32    */
  /* kmalloc-96            96         42    */
  /* kmalloc-64            64         64    */
  /* kmalloc-32            32        128    */
  /* kmalloc-16            16        256    */
  /* kmalloc-8              8        512    */


  for (int i = 0; i < 8; ++i) {
    println("[Kalloc:Init] register allocator for size %d", default_size_list[i]);
    register_allocator(default_size_list[i], 1);
  }
}
