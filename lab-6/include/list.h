#ifndef __LIST_H
#define __LIST_H

#ifndef __ASSEMBLER__

typedef struct list {
    struct list *next, *prev; //double link list
} List;

#define offsetof(type, member) ((unsigned long) &((type *)0)->member)

#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
		(type *)( (char *)__mptr - offsetof(type, member) );  \
})

#define list_entry(ptr,type,member)	\
    container_of(ptr, type, member)

#define LIST_INIT(ptr)  \
do { \
    (ptr)->next = (ptr); \
    (ptr)->prev = (ptr); \
} while (0)

static inline void __list_add(List *new_list, \
                            List *prev, \
                            List *next)
{
	next->prev = new_list;
	new_list->next = next;
	new_list->prev = prev;
	prev->next = new_list;
}

static inline void list_add_tail(List *new_list, \
                                List *head)
{
	__list_add(new_list, head->prev, head);
}

static inline int is_list_empty(List *new_list){
    return new_list == new_list->next;
}

// add chain between head and head->next
static inline void list_add_chain(List *add_head, 
                                List *add_end,
                                List *be_added_list){
    add_head->prev = be_added_list;
    add_end->next = be_added_list->next;
    be_added_list->next->prev = add_end;
    be_added_list->next = add_head;
}

// add chain in list tail
static inline void list_add_chain_tail(List *add_head, \
                                        List *add_end, \
		                                List *be_added_list)
{
    add_head->prev = be_added_list;
    add_end->next = be_added_list->next;
    be_added_list->next->prev = add_end;
    be_added_list->next = add_head;
}

//link between (node which ahead chain) and (node which behind chain)
static inline void list_remove_chain(List *chain_head, \
                                    List *chain_end) {
	chain_head->prev->next = chain_end->next;
	chain_end->next->prev = chain_head->prev;
}

#endif

#endif  /*_LIST_H */