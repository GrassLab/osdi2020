#ifndef KERNEL_LIB_LIST_
#define KERNEL_LIB_LIST_

#include "kernel/lib/types.h"

struct list_head {
  struct list_head *prev;
  struct list_head *next;
};

#define container_of(ptr, type, member) ({            \
  const typeof(((type *)0)->member) *__mptr = (ptr);  \
  (type *) ((char *)__mptr - offsetof(type, member)); \
})

#define list_entry(ptr, type, member) container_of(ptr, type, member)

void init_list_head(struct list_head *head);
bool list_empty(struct list_head *head);
void list_add_head(struct list_head *new, struct list_head *head);
void list_add_tail(struct list_head *new, struct list_head *head);
struct list_head *list_del_head(struct list_head *head);
void list_del(struct list_head *entry);


#endif // KERNEL_LIB_LIST_