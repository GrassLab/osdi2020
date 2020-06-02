#ifndef __MM_H__
#define __MM_H__

#define KERNEL_VIRT_BASE        0xFFFF000000000000
#define PAGE_SIZE               4096
#define PAGE_FRAMES_NUM         (0x40000000 / PAGE_SIZE)
#define PAGE_MASK               ~0xFFF
#define MAX_BUDDY_ORDER         9 // 2^0 ~ 2^8 => 4k to 1MB

#ifndef __ASSEMBLY__

#include "typedef.h"
#include "schedule.h"

enum page_status {
    AVAL,
    USED,
};

struct list_head {
    struct list_head *next, *prev;
};

struct buddy_t {
    uint32_t nr_free;
    struct list_head head;
};

struct page_t {
    struct list_head list;
    enum page_status used;
    int order;
    int idx; // debug used
};

/* Variables init in mm.c */
extern struct buddy_t free_area[MAX_BUDDY_ORDER];
extern struct page_t page[PAGE_FRAMES_NUM];
extern uint64_t remain_page;

/* Function in mm.c */
void mm_init();
void* page_alloc();
void fork_pgd(struct task_t* target, struct task_t* dest);
void* map_page(struct task_t* task, uint64_t user_addr);
void page_reclaim(uint64_t pgd_phy);

uint64_t user_addr_to_page_addr(uint64_t user_addr, uint64_t pgd);
void memzero(uint8_t* addr, int size);
void memcpy(void *dest, void *src, uint64_t size);
uint64_t virtual_to_physical(uint64_t virt_addr);
uint64_t phy_to_pfn(uint64_t phy_addr);

#endif /* __ASSEMBLY__ */

#endif /* __MM_H__ */