#pragma once

struct Pair {
  int lb, ub;
};

struct Node {
  struct Pair pair;
  struct Node *next;
};

struct List {
  struct Node *start;
  struct List *list_next;
};

extern unsigned long base;

void insert_node(struct Node **head, struct Pair p);
void insert_list(struct List **Listhead, struct Node *head);
void show(struct List *Listhead);
void show_nodes(struct Node *head);
struct Pair remove_node(struct Node **head);
struct List *list_at(struct List *ListHead, int index);
