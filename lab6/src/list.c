#include "list.h"
#include "mm.h"
#include "printf.h"

unsigned long base = 0;

void insert_node(struct Node **head, struct Pair p) {
  /* the base of new ndoe is locate at (base + low-bound * 4KB)  */
  /* because the (lb, ub) is corresponding to memory area of [lb * 4kB, ub * 4kB]*/
  struct Node *temp = (struct Node *)(base + (p.lb * PAGE_SIZE));

  temp->pair = p;

  /* point to the head */
  temp->next = (*head);

  /* transfer head to current node */
  (*head) = temp;
}

struct Pair remove_node(struct Node **head) {
  struct Node *temp = (*head);

  /* node exist */
  if (temp) {
    struct Pair p = temp->pair;

    /* transfer head to next */
    (*head) = temp->next;

    //free_page((unsigned long)temp + VA_START);

    return p;
  }

  /* otherwise */
  return (struct Pair){};
}

void show_nodes(struct Node *head) {
  struct Node *cur = head;
  while (cur != 0) {
    printf("(");
    printf("%d,", cur->pair.lb);
    printf("%d) ", cur->pair.ub);
    cur = cur->next;
  }
}
