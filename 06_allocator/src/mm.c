#include "mm.h"
#include "peripherals/uart.h"

unsigned short memory_map[NUM_PAGES] = {0,};
struct list_head page_buddy[MAX_BUDDY_PAGE_NUM];

#define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({          \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define BUDDY_END(x, order)         ((x) + (1<<(order)) - 1)
#define NEXT_BUDDY_START(x, order)  ((x) + (1<<(order)))
#define PREV_BUDDY_START(x, order)  ((x) - (1<<(order)))

/* simple allocate a continuous memory */
unsigned long allocate_task_struct() {
    for (int i = 0; i < NUM_PAGES; ++ i) {
        if (memory_map[i] == 0) {
            memory_map[i] = 1;
            return LOW_MEMORY + i * PAGE_SIZE;
        }
    }
    // 0 means allocate failed
    return 0;
}

void free_task_struct(unsigned long ptr) {
    memory_map[(ptr - LOW_MEMORY) / PAGE_SIZE] = 0;
}

static inline void __list_add(struct list_head *new_lst,
                              struct list_head *prev,
                              struct list_head *next) {
    next->prev = new_lst;
    new_lst->next = next;
    new_lst->prev = prev;
    prev->next = new_lst;
}

static inline void list_add_tail(struct list_head *new_lst, struct list_head *head) {
    __list_add(new_lst, head->prev, head);
}

static inline void list_add_chain(struct list_head *ch, struct list_head *ct, struct list_head *head) {
    ch->prev = head;
    ct->next = head->next;
    head->next->prev = ct;
    head->next = ch;
}

static inline void list_add_chain_tail(struct list_head *ch, 
                                       struct list_head *ct,
                                       struct list_head *head) {
    ch->prev=head->prev;
    head->prev->next=ch;
    head->prev=ct;
    ct->next=head;
}

static inline void list_remove_chain(struct list_head *ch, struct list_head *ct) {
    ch->prev->next = ct->next;
    ct->next->prev = ch->prev;
}

static inline int list_empty(const struct list_head *head) {
    return head->next == head;
}

static inline void INIT_LIST_HEAD(struct list_head *list) {
    list->next = list;
    list->prev = list;
}

void init_page_buddy() {
    for (int i = 0; i < MAX_BUDDY_PAGE_NUM; ++ i) {
        INIT_LIST_HEAD(&page_buddy[i]);
    }
}

void init_page_map() {
    struct page *pg = (struct page*) KERNEL_PAGE_START;
    uart_puts("Init page map\n");
    uart_puts("Kernel paging from ");
    uart_send_hex(KERNEL_PAGING_START);
    uart_puts(" to ");
    uart_send_hex(KERNEL_PAGING_END);
    uart_puts("\nKernel page struct start: ");
    uart_send_hex(KERNEL_PAGE_START);
    uart_send('\n');

    init_page_buddy();

    for (int i = 0; i < KERNEL_PAGE_NUM; pg++, i++) {
        pg->vaddr = KERNEL_PAGING_START + i * PAGE_SIZE;
        pg->flags = PAGE_AVAILABLE;
        pg->counter = 0;
        INIT_LIST_HEAD(&(pg->list));
    
        /* calculate the number which is the multiplier of 512 and smaller than KERNEL_PAGE_NUM */
        if (i < (KERNEL_PAGE_NUM & (~PAGE_NUM_FOR_MAX_BUDDY))) {
            if ((i & PAGE_NUM_FOR_MAX_BUDDY) == 0) {
                pg->order = MAX_BUDDY_PAGE_NUM - 1;
            } else {
                /* remaining page mark as -1 */
                pg->order = -1;
            }
            list_add_tail(&(pg->list), &page_buddy[MAX_BUDDY_PAGE_NUM - 1]);
        } else {
            pg->order = 0;
            list_add_tail(&(pg->list), &page_buddy[0]);
        }
    }
}

struct page* get_pages_from_list(int order) {
    int new_order = order;
    struct page *pg;
    struct list_head *tlst, *tlst1;
    for (; new_order < MAX_BUDDY_PAGE_NUM; new_order ++) {
        /* current order is empty, ask for next order */
        if (list_empty(&page_buddy[new_order])) {
            uart_puts("page_buddy[");
            uart_send_ulong(new_order);
            uart_puts("] is empty, search for next order\n");
            continue;
        } else {
            uart_puts("Find a buddy larger than requested at order ");
            uart_send_ulong(new_order);
            uart_puts(", addr: ");
            /* get the head of first buddy */
            pg = list_entry(page_buddy[new_order].next, struct page, list);
            uart_send_hex((unsigned long)pg);
            uart_send('\n');
            /* get the end of first buddy */
            tlst = &(BUDDY_END(pg, new_order)->list);
            tlst->next->prev = &page_buddy[new_order];
            uart_puts("Reconnect the prev of ");
            uart_send_hex((unsigned long)(list_entry(tlst->next, struct page, list)));
            uart_puts(" to page_buddy[");
            uart_send_ulong(new_order);
            uart_puts("], and the next of page_buddy[");
            uart_send_ulong(new_order);
            uart_puts("] to");
            page_buddy[new_order].next = tlst->next;
            uart_send_hex((unsigned long)(list_entry(tlst->next, struct page, list)));
            uart_send('\n');
            goto PAGE_POST;
        }
    }
    return NULL;

PAGE_POST:
    for (new_order --; new_order >= order; --new_order) {
        /* remain first half of page in this order */
        tlst1 = &(BUDDY_END(pg, new_order)->list);
        tlst = &(pg->list);
        uart_puts("Set return addr: ");
        pg = NEXT_BUDDY_START(pg, new_order);
        uart_send_hex((unsigned long)pg);
        uart_send('\n');
        list_entry(tlst, struct page, list)->order = new_order;
        uart_puts("Append ");
        uart_send_hex((unsigned long)list_entry(tlst, struct page, list));
        uart_puts(" to page_buddy[");
        uart_send_ulong(new_order);
        uart_puts("]\n");
        list_add_chain_tail(tlst, tlst1, &page_buddy[new_order]);
    }
    pg->flags |= PAGE_BUDDY_BUSY;
    pg->order = order;
    return pg;
}

void put_pages_to_list(struct page *pg, int order) {
    struct page *tprev, *tnext;
    if (!(pg->flags & PAGE_BUDDY_BUSY)) {
        uart_puts("Not implemented Warning\n");
        return;
    }
    pg->flags &= ~(PAGE_BUDDY_BUSY);
    
    for (; order < MAX_BUDDY_PAGE_NUM; ++ order) {
        tnext = NEXT_BUDDY_START(pg, order);
        tprev = PREV_BUDDY_START(pg, order);
        if ((!(tnext->flags & PAGE_BUDDY_BUSY)) && (tnext->order == order)) {
            uart_puts("merging ");
            uart_send_hex((unsigned long)pg);
            uart_puts(" and ");
            uart_send_hex((unsigned long)tnext);
            uart_send('\n');
            /* merge if avail */
            pg->order ++;
            tnext->order = -1;
            list_remove_chain(&(tnext->list), &(BUDDY_END(tnext, order)->list));
            BUDDY_END(pg, order)->list.next = &(tnext->list);
            tnext->list.prev = &(BUDDY_END(pg, order)->list);
            if (order + 1 == MAX_BUDDY_PAGE_NUM)
                break;
            else
                continue;
        } else if ((!(tprev->flags & PAGE_BUDDY_BUSY)) && (tprev->order == order)) {
            uart_puts("merging ");
            uart_send_hex((unsigned long)pg);
            uart_puts(" and ");
            uart_send_hex((unsigned long)tprev);
            uart_send('\n');
            pg->order = -1;
            list_remove_chain(&(tprev->list), &(BUDDY_END(tprev, order))->list);
            BUDDY_END(tprev, order)->list.next = &(pg->list);
            pg->list.prev = &(BUDDY_END(tprev, order)->list);
            pg = tprev;
            pg->order ++;
            if (order + 1 == MAX_BUDDY_PAGE_NUM)
                break;
            else
                continue;
        } else {
            // no merge is required
            break;
        }
    }
    uart_puts("Append ");
    uart_send_hex((unsigned long)pg);
    uart_puts(" to page_buddy[");
    uart_send_ulong(order);
    uart_puts("]\n");
    list_add_chain(&(pg->list), &(BUDDY_END(pg, order)->list), &page_buddy[order]);
}

void* page_address(struct page *pg) {
    return (void *)(pg->vaddr);
}

void show_part_buddy(struct list_head *ptr) {
    struct list_head *end = ptr;

    while (ptr->next != end) {
        struct page *pg = list_entry(ptr, struct page, list);
        if (ptr != end && pg->order > 0) {
            uart_send_hex((unsigned long)pg);
            uart_puts(" -> ");
        }
        ptr = ptr->next;
    }
}

void show_buddy_system() {
    uart_puts("======BUDDY STATUS======\n");
    for (int i = 0; i < MAX_BUDDY_PAGE_NUM; ++ i) {
        uart_send_ulong(i);
        uart_puts(": ");
        show_part_buddy(&page_buddy[i]);
        uart_send('\n');
    }
    uart_puts("========================\n");
}

struct page* alloc_pages(unsigned long flag, int order) {
    struct page *pg;
    pg = get_pages_from_list(order);
    if (!pg) return NULL;
    for (int i = 0; i < (1<<order); ++i) {
        (pg + i)->flags |= PAGE_DIRTY;
    }
    return pg;
}

void* get_free_pages(unsigned long flag, int order) {
    uart_puts("Request a continuous memory with order ");
    uart_send_ulong(order);
    uart_send('\n');
    struct page *page;
    page = alloc_pages(flag, order);
    if (!page) return NULL;
    return page_address(page);
}

void free_pages(struct page *pg, int order) {
    uart_puts("Return page addr ");
    uart_send_hex((unsigned long)pg);
    uart_puts(" with order ");
    uart_send_ulong(order);
    uart_send('\n');
    for (int i = 0; i < (1<<order); ++i) {
        (pg + i)->flags &= (~PAGE_DIRTY);
    }
    put_pages_to_list(pg, order);
}

struct page* virt_to_page(unsigned long addr) {
    unsigned long i = ((addr) - KERNEL_PAGING_START) >> PAGE_SHIFT;
    if (i > KERNEL_PAGE_NUM) return NULL;
    return (struct page*) KERNEL_PAGE_START + i;
}

void put_free_pages(void *addr, int order) {
    free_pages(virt_to_page((unsigned long)addr), order);
}

char allocator_used_map[NUM_ALLOCATORS];
struct allocator allocator_pool[NUM_ALLOCATORS];

void pool_init(struct allocator* allocator, unsigned long size) {
    allocator->chunk_size = size;
    allocator->base_addr = (unsigned long)get_free_pages(PAGE_AVAILABLE, ALLOCATOR_ORDER);
    unsigned long page_size_from_buddy = (1 << ALLOCATOR_ORDER) * PAGE_SIZE;
    unsigned long num_chunk = page_size_from_buddy / (1 + size);
    allocator->num_chunk = num_chunk;

    // first num_chunk for recording the usage
    for (int i = 0; i < num_chunk; ++ i) {
        ((char*)(allocator->base_addr))[i] = 0;
    }
    allocator->addr = allocator->base_addr + num_chunk;
}

/* fixed sized allocator */
unsigned long allocator_register(unsigned long size) {
    for (int i = 0; i < NUM_ALLOCATORS; ++ i) {
        if (!allocator_used_map[i]) {
            allocator_used_map[i] = 1;
            pool_init(allocator_pool + i, size);
            return i;
        }
    }
    // error 
    return -1;
}

unsigned long allocator_alloc(unsigned long allocator_id) {
    struct allocator* ar = allocator_pool + allocator_id;
    char* used_map = (char*)ar->base_addr;
    for (int i = 0; i < ar->num_chunk; ++ i) {
        if (!used_map[i]) {
            used_map[i] = 1;
            return ar->addr + ar->chunk_size * i;
        }
    }
    // error
    return 0;
}

void allocator_free(unsigned long allocator_id, unsigned long addr) {
    // assume user pass the correct addr
    struct allocator* ar = allocator_pool + allocator_id;
    char* used_map = (char*)ar->base_addr;
    unsigned long idx = (addr - ar->base_addr) / ar->chunk_size;
    used_map[idx] = 0;
}

void allocator_unregister(unsigned long allocator_id) {
    struct allocator* ar = allocator_pool + allocator_id;
    put_free_pages((void*)ar->base_addr, ALLOCATOR_ORDER);
    allocator_used_map[allocator_id] = 0;
}
