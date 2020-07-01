#pragma once

struct Pair {
  unsigned long lb, ub;
};

struct Node {
  struct Pair pair;
  struct Node *next;
};

extern unsigned long base;

void insert_node(struct Node **head, struct Pair p);
void show_nodes(struct Node *head);
struct Pair remove_node(struct Node **head);
