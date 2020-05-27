#include "printf.h"
#include "mm.h"
#include "buddy.h"

int cal_index(int size) {
  int targetlevel = 0;
  while (size >>= 1) ++targetlevel;
  return targetlevel;
}

struct buddy *buddy_new(int size, unsigned long base_d) {
  struct buddy *temp = (struct buddy *)allocate_kernel_page();

  /* calcuate the index of power of 2 */
  int x = cal_index(size);

  /* init memory */
  temp->len = x + 1;
  temp->size = size;

  /* the base of the memory which buddy would located */
  base = base_d;
  temp->pair_array = (struct Node **)allocate_kernel_page();

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

struct Pair buddy_allocate(struct buddy *self, int size) {
  /* calcuate the index of power of 2 */
  int x = cal_index(size);

  struct Pair temp;

  struct Node **arr = &self->pair_array[x];
  if (self->pair_array[x] != 0) {
    temp = remove_node(arr);
    return temp;
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
    printf("[Error:Alloc] failed to allocate memory");
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

    temp = remove_node(arr);
  }

  return temp;
}

struct buddy_type Buddy = {
    .construct = buddy_construct,
    .new = buddy_new,
    .show = buddy_show,
    .alloc = buddy_allocate,
};
