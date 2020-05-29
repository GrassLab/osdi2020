#include "buddy.h"
#include "mm.h"
#include "printf.h"

struct buddy *global_bd = 0;


int cal_index(int size) {
  int targetlevel = 0;
  int add1_flag = 0;
  while (size >> 1) {

    if (size & 1)
      add1_flag = 1;

    ++targetlevel;
    size >>= 1;
  }

  return targetlevel + add1_flag;
}

struct buddy *buddy_new(int size, unsigned long base_d) {
  /* create the buddy item & adjust the new base */
  struct buddy *temp = (struct buddy *)base_d;
  base_d += sizeof(struct buddy);

  /* calcuate the index of power of 2 */
  int x = cal_index(size);

  /* init memory */
  temp->len = x + 1;
  temp->size = size;

  /* assign the free list array & adjust the new base */
  temp->pair_array = (struct Node **)base_d;
  base_d += (sizeof(struct Node) * temp->len);

  /* assign the hash map for furture deallocate & zero out */
  /* it's just the linear map */
  temp->map = (unsigned long *)base_d;
  unsigned long map_offset = (sizeof(unsigned long) * size);
  base_d += map_offset;
  memzero((unsigned long)temp->map, map_offset);

  /* the base of the memory which buddy would located */
  base = base_d;

  /* init the index at len */
  struct Node **cur = &temp->pair_array[x];
  struct Pair p = {0, size - 1};
  insert_node(cur, p);

  return temp;
}

void buddy_construct(struct buddy *self) {} /* not used in this moment */

void buddy_show(struct buddy *self) {
  struct Node *cur = 0;
  for (int i = 0; i < self->len; ++i) {
    printf("List %d: ", i);
    cur = self->pair_array[i];
    show_nodes(cur);
    printf("\n");
  }
}

int bd_phy2n(unsigned long addr) {
  return (addr - base) >> 12;
}


struct Pair pair_shift(struct Pair p, int shift) {
  return (struct Pair){base + (p.lb << shift), base + (p.ub << shift)};
}

struct Pair buddy_allocate(struct buddy *self, int size) {
  /* calcuate the index of power of 2 */
  int x = cal_index(size);

  struct Pair temp;

  struct Node **arr = &self->pair_array[x];
  if (self->pair_array[x] != 0) {
    temp = remove_node(arr);

    /* put the lb, and size into map */
    self->map[temp.lb] = temp.ub - temp.lb + 1;
    /* Buddy.show(self); */
    return pair_shift((struct Pair){temp.lb, temp.ub}, 12);
  }

  /* search for a larger block */
  int i;
  for (i = x + 1; i < self->len; ++i) {
    if (self->pair_array[i] == 0)
      continue;
    /* found a larger block */
    break;
  }

  if (i == self->len) {
    println("[Error:Alloc] failed to allocate memory");
    return (struct Pair){};
  }

  /* remove the first block */
  arr = &self->pair_array[i];

  /* remove the first block from arr[i] */
  temp = remove_node(arr);
  i--;

  /* traverse down the list */
  for (; i >= x; i--) {
    arr = &self->pair_array[i];

    /* divide the block in two halves */
    struct Pair p1 = {temp.lb, temp.lb + (temp.ub - temp.lb) / 2};
    struct Pair p2 = {temp.lb + (temp.ub - temp.lb + 1) / 2, temp.ub};

    insert_node(arr, p2);
    insert_node(arr, p1);

    self->map[p1.lb] = p1.ub - p1.lb + 1;
    self->map[p2.lb] = p2.ub - p2.lb + 1;

    /* Buddy.show(self);  */
    temp = remove_node(arr);
    self->map[temp.lb] = temp.ub - temp.lb + 1;
  }

  /* put the lb, and size into map */
  self->map[temp.lb] = temp.ub - temp.lb + 1;
  /* Buddy.show(self); */
  return pair_shift((struct Pair){temp.lb, temp.ub}, 12);
}

void try_coalescing(struct buddy *self, int s) {
  unsigned long block_size = self->map[s];
  int x = cal_index(block_size);

  /* Idea:                                                 */
  /* +----------------------------------------------       */
  /* |0              31|32    47|48      63|64 ...         */
  /* +---------\-------------\------/-------------         */
  /*            \             \    /                       */
  /*             \             \  /                        */
  /*              \             \/                         */
  /*               \____    ____/                          */
  /*                    \  /                               */
  /*                     \/                                */
  /*                      \___...                          */

  /* block number */
  int bN = s / block_size;
  /* the buddy address of the s */
  /* it's a pair with the original s */
  int bA = s + (bN & 1 ? -(1 << x) : 1 << x);

  println("s: %d, bN: %d, bA: %d, x: %d", s, bN, bA, x);

  /* search free list for buddy */
  struct Node **arr_x = &self->pair_array[x];

  /* trace the previous node for remove node */
  struct Node *pre = *arr_x;
  for (struct Node *cur = *arr_x; cur != 0; cur = cur->next) {
    /* buddy is also free */
    if ((*cur).pair.lb == bA) {
      /* check size */
      /* if the size of two item is not same */
      if (self->map[cur->pair.lb] != block_size) {
        return;
      }

      /* if (self->map[cur->pair.lb] != block_size)
       * {println("wired!!!!!!!!!!!"); continue;} */
      println("vvvvvvvvvvvvv pre coalescing vvvvvvvvvvvv");
      Buddy.show(self);
      println("-----------------------------------------");

      /* buddy is the block after block */
      struct Node **next_block = &self->pair_array[x + 1];
      int bd = bN & 1 ? bA : s;
      struct Pair new_pair = (struct Pair){bd, bd + 2 * (1 << x) - 1};

      println("[Coalesce] %d & %d", bA, s);

      /* remove inidividual segements */
      /* as they have coalesced  */

      /* because of the first node must be the item we need coalesced */
      /* so here, the pre node must not same as the current node we meet */
      /* because the current node is the later node we need to merge */
      /* so just ignore it */
      pre->next = cur->next;

      /* and just dump the first node, because that what we want to coalesce */
      struct Pair removed = remove_node(arr_x);

      /* don't forget set the two item in map we removed above to 0 */
      self->map[cur->pair.lb] = self->map[removed.lb] = 0;

      /* finally, insert the sum node into next block & try to coalesce again */
      insert_node(next_block, new_pair);
      self->map[new_pair.lb] = 2 * (1 << x);
      return try_coalescing(self, new_pair.lb);
    }

    pre = cur;
  }
  return;
}

void buddy_deallocate(struct buddy *self, int s) {
  /* check reuqest */
  unsigned long used = self->map[s];

  if (!used) {
    println("[Error:dealloc] Invalid free request");
    return;
  }

  /* get the index of pair_array */
  int x = cal_index(used);

  /* add it to free list */
  insert_node(&self->pair_array[x], (struct Pair){s, s + (1 << x) - 1});

  println("[Free] page %d to %d", s, s + (1 << x) - 1);

  /* try coalescing */
  /* calculate buddy number & address */
  return try_coalescing(self, s);
}

struct buddy_type Buddy = {
    .construct = buddy_construct,
    .new = buddy_new,
    .show = buddy_show,
    .alloc = buddy_allocate,
    .dealloc = buddy_deallocate,
};
