#ifndef __MM_H__
#define __MM_H__

#define KERNEL_VIRT_BASE        0xFFFF000000000000
#define PAGE_SIZE               4096
#define PAGE_FRAMES_NUM         (0x40000000 / PAGE_SIZE)

#ifndef __ASSEMBLY__

#include "typedef.h"
#include "schedule.h"

enum page_status {
    AVAL,
    USED,
};

struct page_t {
    enum page_status used;
};

/* Variables init in mm.c */
extern struct page_t page[PAGE_FRAMES_NUM];
extern uint64_t remain_page;

/* Function in mm.c */
void mm_init();
void memcpy(void *dest, void *src, uint64_t size);
uint64_t virtual_to_physical(uint64_t virt_addr);
uint64_t phy_to_pfn(uint64_t phy_addr);
void* page_alloc(struct task_t* task);
void* page_alloc_user(struct task_t* task, uint64_t user_addr);
void page_free(void* addr);
void* get_page_user(struct task_t* task, uint64_t user_addr);
uint64_t user_addr_to_page_addr(uint64_t user_addr, uint64_t pgd);

#endif /* __ASSEMBLY__ */

#endif /* __MM_H__ */