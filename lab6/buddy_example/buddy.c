#include "list.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int cal_index(int size) {
  int targetlevel = 0;
  while (size >>= 1) ++targetlevel;
  return targetlevel;
}

struct buddy {
  struct buddy_type *self;
  /* private */
  int size;
  int len;
  struct Node **pair_array;
};

struct buddy *buddy_new(int size) {
  struct buddy *temp = (struct buddy *)malloc(sizeof(struct buddy));

  /* calcuate the index of power of 2 */
  int x = cal_index(size);

  /* init memory */
  temp->len = x + 1;
  temp->size = size;
  base = (unsigned long)malloc(size * (2 << 12));
  temp->pair_array = (struct Node **)malloc(sizeof(struct Node) * temp->len);

  /* init the index at len */
  struct Node **cur = &temp->pair_array[x];
  struct Pair p = {0, size - 1};
  insert_node(cur, p);

  return temp;
}

void buddy_construct(struct buddy *self) {}

void buddy_show(struct buddy *self) {
  struct Node *cur = NULL;
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
  if (self->pair_array[x] != NULL) {
    temp = remove_node(arr);
    return temp;
  }

  /* search for a larger block */
  int i;
  for (i = x + 1; i < self->len; ++i) {
    if (self->pair_array[i] == NULL)
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

struct buddy_type {
  void (*construct)(struct buddy *self);
  struct buddy *(*new)(int size);
  void (*show)(struct buddy *self);
  struct Pair (*alloc)(struct buddy *self, int size);
} Buddy = {
    .construct = buddy_construct,
    .new = buddy_new,
    .show = buddy_show,
    .alloc = buddy_allocate,
};

struct Pair extend_4kb(struct Pair p) {
  return (struct Pair){ p.lb << 12, p.ub << 12 };
}


int main(int argc, char *argv[]) {

  struct buddy *bd = Buddy.new(128);
  if (!bd) {
    printf("erorr while constructing the buddy system");
    return -1;
  }
  /* status of init bd */
  Buddy.show(bd);
  struct Pair p = extend_4kb(Buddy.alloc(bd, 32));
  printf("alloc %x to %x\n", p.lb, p.ub);
  Buddy.show(bd);

  p = extend_4kb(Buddy.alloc(bd, 7));
  printf("alloc %x to %x\n", p.lb, p.ub);
  Buddy.show(bd);

  p = extend_4kb(Buddy.alloc(bd, 64));
  printf("alloc %x to %x\n", p.lb, p.ub);
  Buddy.show(bd);

  printf("dd: %d", sizeof (struct Node));

  return 0;
}
