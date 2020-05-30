#ifndef __LIST_H
#define __LIST_H

#ifndef __ASSEMBLER__

struct list_pointer {
    struct list_pointer *next, *prev;
};

#define INIT_LIST_POINTER(ptr) do { \
        (ptr)->next = (ptr); \
        (ptr)->prev = (ptr); \
} while (0)

static inline void __list_add(struct list_pointer *new_lst, struct list_pointer *prev, struct list_pointer *next)
{
	next->prev = new_lst;
	new_lst->next = next;
	new_lst->prev = prev;
	prev->next = new_lst;
}

static inline void list_add_tail(struct list_pointer *new_lst, struct list_pointer *head)
{
	__list_add(new_lst, head->prev, head);
}

#endif

#endif  /*_LIST_H */