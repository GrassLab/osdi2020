#include "typedef.h"

struct list_head {
    struct list_head *next, *prev;
};

#define offsetof(TYPE, MEMBER) ((uint64_t)&((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
               (type *)( (char *)__mptr - offsetof(type,member) ); })

#define list_entry(ptr, type, member) container_of(ptr, type, member)

void list_head_init(struct list_head *l);
int list_empty(struct list_head *l);
void list_add(struct list_head *new_lst, struct list_head *head);
void list_add_tail(struct list_head *new_lst, struct list_head *head);
void list_del(struct list_head *entry);