#ifndef __LIST_H
#define __LIST_H

#ifndef __ASSEMBLER__

struct list_pointer {
    struct list_pointer *next, *prev; //double link list
};
typedef struct list_pointer list_ptr_t;

#define offsetof(type, member) ((unsigned long) &((type *)0)->member)

#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
		(type *)( (char *)__mptr - offsetof(type, member) );  \
})

#define list_entry(ptr,type,member)	\
    container_of(ptr, type, member)

#define INIT_LIST_POINTER(ptr) do { \
    (ptr)->next = (ptr); \
    (ptr)->prev = (ptr); \
} while (0)

static inline void __list_add(struct list_pointer *new_list, struct list_pointer *prev, struct list_pointer *next)
{
	next->prev = new_list;
	new_list->next = next;
	new_list->prev = prev;
	prev->next = new_list;
}

static inline void list_add_tail(struct list_pointer *new_list, struct list_pointer *head)
{
	__list_add(new_list, head->prev, head);
}

static inline int is_list_empty(struct list_pointer *new_list){
    return new_list->prev == new_list->next;
}

#endif

#endif  /*_LIST_H */