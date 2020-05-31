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

#define INIT_LIST_POINTER(ptr)  \
do { \
    (ptr)->next = (ptr); \
    (ptr)->prev = (ptr); \
} while (0)

static inline void __list_add(list_ptr_t *new_list, \
                            list_ptr_t *prev, \
                            list_ptr_t *next)
{
	next->prev = new_list;
	new_list->next = next;
	new_list->prev = prev;
	prev->next = new_list;
}

static inline void list_add_tail(list_ptr_t *new_list, \
                                list_ptr_t *head)
{
	__list_add(new_list, head->prev, head);
}

static inline int is_list_empty(list_ptr_t *new_list){
    return new_list == new_list->next;
}

//If we have one element
// |---------------------|           |---------------------|    |---------------------|
// |      add_head       | .......-> |      add_end        | -> | be_added_list(head) |
// |---------------------| .......<- |---------------------| <- |---------------------|
//If we have two element
// |---------------------|           |---------------------|    |---------------------| .......   |---------------------|    |---------------------|
// |      add_head1      | .......-> |      add_end1       | -> |      add_head2      | .......-> |      add_end2       | -> | be_added_list(head) |
// |---------------------| .......<- |---------------------| <- |---------------------| .......<- |---------------------| <- |---------------------|
static inline void list_add_chain_tail(list_ptr_t *add_head, \
                                        list_ptr_t *add_end, \
		                                list_ptr_t *be_added_list)
{
    add_head->prev=be_added_list;
    add_end->next=be_added_list->next;
    be_added_list->next->prev=add_end;
    be_added_list->next=add_head;
}

#endif

#endif  /*_LIST_H */