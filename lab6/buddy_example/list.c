#include "list.h"
#include <stdio.h>
#include <stdlib.h>

unsigned long base = 0;

void insert_node(struct Node **head, struct Pair p) {
  struct Node *temp = (struct Node *)(base + (p.lb << 12));

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

    /* free(temp); */
    return p;
  }

  /* otherwise */
  return (struct Pair){};
}

void insert_list(struct List **Listhead, struct Node *head) {
  struct List *temp = malloc(sizeof(struct List));

  temp->start = head;

  /* point to the head  */
  temp->list_next = (*Listhead);

  /* transfer head to temp */
  (*Listhead) = temp;
}

struct List *construct_list(int len) {
  /* construct head */
}

struct List *list_at(struct List *ListHead, int index) {
  struct List *current = ListHead;
  while (current != NULL && index-- > 0) {
    /* move to next */
    current = current->list_next;
  }
  return current;
}

void show_nodes(struct Node *head) {
  struct Node *cur = head;
  while (cur != NULL) {
    printf("(");
    printf("%d,", cur->pair.lb);
    printf("%d) ", cur->pair.ub);
    cur = cur->next;
  }
}

void show(struct List *Listhead) {
  int i = 1;
  struct List *current;
  struct Node *currentlist;
  current = Listhead;
  while (current != NULL) {
    currentlist = current->start;
    printf("List %d: ", i);
    while (currentlist != NULL) {
      printf("(");
      printf("%d,", currentlist->pair.lb);
      printf("%d) ", currentlist->pair.ub);
      currentlist = currentlist->next;
    }
    i++;
    printf("\n");
    current = current->list_next;
  }
}

/* int main(int argc, char *argv[]) { */
/*   struct Node *head1 = NULL, *head2 = NULL, */
/*               *head3 = NULL;    // 3 lists of integers */
/*   struct List *Listhead = NULL; // Listhead will be the head of lists of list
 */

/*   struct Pair p1 = {1, 1}; */
/*   struct Pair p2 = {2, 2}; */
/*   struct Pair p3 = {3, 3}; */
/*   struct Pair p4 = {4, 4}; */
/*   struct Pair p5 = {5, 5}; */
/*   struct Pair p6 = {6, 6}; */
/*   struct Pair p7 = {7, 7}; */
/*   struct Pair p8 = {8, 8}; */
/*   struct Pair p9 = {9, 9}; */

/*   insert_node(&head1, p1); // inserting in first list */
/*   insert_node(&head1, p2); */
/*   insert_node(&head1, p3); */
/*   insert_node(&head1, p4); */
/*   insert_node(&head2, p5); // inserting in second list */
/*   insert_node(&head2, p6); */
/*   insert_node(&head3, p7); // inserting in third list */
/*   insert_node(&head3, p8); */
/*   insert_node(&head3, p9); */
/*   insert_list(&Listhead, head1); // inserting lists in list */
/*   insert_list(&Listhead, head2); */
/*   insert_list(&Listhead, head3); */
/*   show(Listhead); */

/*   printf("List head; %x", Listhead); */

/*   return 0; */
/* } */
