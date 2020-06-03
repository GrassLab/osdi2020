#include "list.h"

void init_list_head(struct list_head *head) {
  head->next = head;
  head->prev = head;
}

bool list_empty(struct list_head *head) {
  return head->next == head;
}

void list_add_head(struct list_head *new, struct list_head *head) {
  struct list_head *next = head->next;
  new->next = next;
  new->prev = head;
  head->next = new;
  next->prev = new;
}

void list_add_tail(struct list_head *new, struct list_head *head) {
  struct list_head *prev = head->prev;
  new->next = head;
  new->prev = prev;
  head->prev = new;
  prev->next = new;
}

struct list_head *list_del_head(struct list_head *head) {
  if (list_empty(head)) {
    return NULL;
  }
  struct list_head *victim = head->next;
  victim->next->prev = head;
  head->next = victim->next;
  *victim = (struct list_head){NULL, NULL};
  return victim;
}

void list_del(struct list_head *entry) {
  entry->prev->next = entry->next;
  entry->next->prev = entry->prev;
}