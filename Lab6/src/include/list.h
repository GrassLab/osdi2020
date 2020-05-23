#ifndef __LIST_H
#define __LIST_H

#ifndef __ASSEMBLER__

struct list_head {
        struct list_head *next, *prev;
};

#define INIT_LIST_HEAD(ptr) do { \
        (ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)
 

static inline void __list_add(struct list_head *new_lst,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new_lst;
	new_lst->next = next;
	new_lst->prev = prev;
	prev->next = new_lst;
}

static inline void list_add_tail(struct list_head *new_lst, struct list_head *head)
{
	__list_add(new_lst, head->prev, head);
}

#endif
#endif
