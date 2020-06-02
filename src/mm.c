#include "mm.h"
#include "mmu.h"

#include "peripherals/mmio.h"
#include "schedule.h"
#include "uart0.h"

struct buddy_t free_area[MAX_BUDDY_ORDER];
struct page_t page[PAGE_FRAMES_NUM];
int first_aval_page, last_aval_page;
uint64_t remain_page = 0;

void list_add(struct list_head* elmt, struct list_head* prev, struct list_head* next) {
	next->prev = elmt;
	elmt->next = next;
	elmt->prev = prev;
	prev->next = elmt;
}

void buddy_push(struct buddy_t* bd, struct list_head* elmt) {
    list_add(elmt, bd->head.prev, &bd->head);
    bd->nr_free++;
}

struct page_t* buddy_pop(struct buddy_t* bd) {
    if (bd->head.next == &bd->head) return NULL;
    bd->nr_free--;
    struct list_head* target = bd->head.next;
    target->next->prev = target->prev;
    target->prev->next = target->next;
    target->next = NULL;
    target->prev = NULL;
    return (struct page_t*)target; // list_head is the first member of the structure
}

void buddy_remove(struct buddy_t* bd, struct list_head* elmt) {
    bd->nr_free--;
    elmt->prev->next = elmt->next;
    elmt->next->prev = elmt->prev;
    elmt->prev = NULL;
    elmt->next = NULL;
}

void buddy_info() {
    for (int i = 0; i < MAX_BUDDY_ORDER; i++) {
        uart_printf("order: %d %d\n", i, free_area[i].nr_free);
    }
}

struct page_t* buddy_release_redundant(struct page_t* p, int target_order) {
    uart_printf("release order: %d targer: %d\n", p->order, target_order);
    int cur_order = p->order;
    // recursive release if not reach to target_order
    if (cur_order > target_order) {
        int prev_order = cur_order - 1;
        int prev_order_pages = 1 << prev_order;
        struct page_t* bottom = p;
        struct page_t* top = p + prev_order_pages;
        // push bottom half back to buddy system
        for (int i = 0; i < (1 << cur_order); i++) {
            (p + i)->order = prev_order;
        }
        buddy_push(&(free_area[prev_order]), &(bottom->list));
        return buddy_release_redundant(top, target_order);
    }
    // book keeping
    for (int i = 0; i < (1 << cur_order); i++) {
        (p + i)->used = USED;
    }
    return p;
}

void* buddy_alloc(int order) {
    for (int i = order; i < MAX_BUDDY_ORDER; i++) {
        if (free_area[i].nr_free > 0) {
            struct page_t* p = buddy_pop(&free_area[i]);
            struct page_t* target_p = buddy_release_redundant(p, order);
            uint64_t page_virt_addr = (target_p->idx * PAGE_SIZE) | KERNEL_VIRT_BASE;
            return (void*)page_virt_addr;
        }
    }
    return NULL;
}

void buddy_merge(struct page_t *bottom, struct page_t *top, int order) {
    uart_printf("merge buddy from order %d to %d (%d %d)\n", order, order+1, bottom->idx, top->idx);
    int new_order = order + 1;
    int buddy_pfn = bottom->idx ^ (1 << new_order);
    struct page_t *buddy = &page[buddy_pfn];
    // check buddy exist in new order, if exist => recursively merge
    if (buddy->used == AVAL && buddy->order == new_order) {
        buddy_remove(&free_area[new_order], &buddy->list);
        if (buddy > bottom)
            buddy_merge(bottom, buddy, new_order);
        else
            buddy_merge(buddy, bottom, new_order);
    }
    // merge bottom and top to same block
    else {
        for (int i = 0; i < (1 << order); i++) {
            (bottom + i)->used = AVAL;
            (bottom + i)->order = new_order;
            (top + i)->used = AVAL;
            (top + i)->order = new_order;
        }
        buddy_push(&(free_area[new_order]), &(bottom->list));
    }
}

void buddy_free(void* virt_addr) {
    int pfn = phy_to_pfn(virtual_to_physical((uint64_t)virt_addr));
    struct page_t *p = &page[pfn];
    int order = p->order;
    int buddy_pfn = pfn ^ (1 << order);
    struct page_t *buddy = &page[buddy_pfn];
    // can not merge, just free current order's pages
    if (buddy->used == USED || buddy->order != order) {
        uart_printf("free but no merge\n");
        for (int i = 0; i < (1 << order); i++) {
            (p + i)->used = AVAL;
        }
        buddy_push(&(free_area[order]), &(p->list));
    }
    // merge buddy iteratively
    else {
        buddy_remove(&free_area[order], &buddy->list);
        if (buddy > p)
            buddy_merge(p, buddy, order);
        else
            buddy_merge(buddy, p, order);
    }
}

void buddy_init() {
    for (int i = 0; i < MAX_BUDDY_ORDER; i++) {
        free_area[i].head.next = &free_area[i].head;
        free_area[i].head.prev = &free_area[i].head;
        free_area[i].nr_free = 0;
    }
}

void mm_init() {
    buddy_init();

    extern uint8_t __kernel_end;  // defined in linker
    uint8_t* kernel_end = &__kernel_end;
    int kernel_end_page = (uint64_t)kernel_end / PAGE_SIZE;
    int mmio_base_page = MMIO_PHYSICAL / PAGE_SIZE;

    int i = 0;
    for (; i <= kernel_end_page; i++) {
        page[i].used = USED;
    }

    int order = MAX_BUDDY_ORDER - 1;
    int counter = 0;
    for (; i < mmio_base_page; i++) {
        // init page counter times
        if (counter) {
            remain_page++;
            page[i].idx = i;
            page[i].used = AVAL;
            page[i].order = order;
            page[i].list.next = NULL;
            page[i].list.prev = NULL;
            counter--;
        }
        // page i can fill current order
        else if (i + (1 << order) - 1 < mmio_base_page) {
            remain_page++;
            page[i].idx = i;
            page[i].used = AVAL;
            page[i].order = order;
            buddy_push(&(free_area[order]), &(page[i].list));
            counter = (1 << order) - 1;
        }
        // reduce order, try again
        else {
            order--;
            i--;
        }
    }

    for (; i < PAGE_FRAMES_NUM; i++) {
        page[i].used = USED;
    }

    first_aval_page = kernel_end_page + 1;
    last_aval_page = mmio_base_page - 1;
    buddy_info();
    void* addr1 = buddy_alloc(0);
    buddy_info();
    void* addr2 = buddy_alloc(0);
    buddy_info();
    buddy_free(addr1);
    buddy_info();
    buddy_free(addr2);
    buddy_info();
}


uint64_t get_free_page() {  // return page physical address
    for (int i = first_aval_page; i < last_aval_page; i++) {
        if (page[i].used == AVAL) {
            page[i].used = USED;
            uint64_t page_virt_addr = i * PAGE_SIZE + KERNEL_VIRT_BASE;
            uint64_t page_phy_addr = i * PAGE_SIZE;
            memzero((uint8_t*)page_virt_addr, PAGE_SIZE);
            return page_phy_addr;
        }
    }
    return 0;
}

// get one page for kernel space
void* page_alloc() {
    uint64_t page_phy_addr = get_free_page();
    if (page_phy_addr == 0) {
        return NULL;
    }
    uint64_t page_virt_addr = page_phy_addr | KERNEL_VIRT_BASE;
    remain_page--;
    return (void*)page_virt_addr;
}

// create pgd, return pgd address
void* create_pgd(struct task_t* task) {
    if (!task->mm.pgd) {
        void* page = page_alloc();
        if (page == NULL) return NULL;
        task->mm.pgd = virtual_to_physical((uint64_t)page);
    }
    return (void*)(task->mm.pgd + KERNEL_VIRT_BASE);
}

// create page table, return next level table
void* create_page_table(uint64_t* table, uint64_t idx) {
    if (table == NULL) return NULL;
    if (!table[idx]) {
        void* page = page_alloc();
        if (page == NULL) return NULL;
        table[idx] = virtual_to_physical((uint64_t)page) | PD_TABLE;
    }
    return (void*)((table[idx] & PAGE_MASK) + KERNEL_VIRT_BASE);
}

// create page, return page address
void* create_page(uint64_t* pte, uint64_t idx) {
    if (pte == NULL) return NULL;
    if (!pte[idx]) {
        void* page = page_alloc();
        if (page == NULL) return NULL;
        pte[idx] = virtual_to_physical((uint64_t)page) | PTE_NORAL_ATTR | PD_ACCESS_PERM_RW;
    }
    return (void*)((pte[idx] & PAGE_MASK) + KERNEL_VIRT_BASE);
}

// allocate new page in user's address table, return page's virtual address
void* map_page(struct task_t* task, uint64_t user_addr) {
    uint64_t pgd_idx = (user_addr & (PD_MASK << PGD_SHIFT)) >> PGD_SHIFT;
    uint64_t pud_idx = (user_addr & (PD_MASK << PUD_SHIFT)) >> PUD_SHIFT;
    uint64_t pmd_idx = (user_addr & (PD_MASK << PMD_SHIFT)) >> PMD_SHIFT;
    uint64_t pte_idx = (user_addr & (PD_MASK << PTE_SHIFT)) >> PTE_SHIFT;

    uint64_t* pgd = create_pgd(task);
    uint64_t* pud = create_page_table(pgd, pgd_idx);
    uint64_t* pmd = create_page_table(pud, pud_idx);
    uint64_t* pte = create_page_table(pmd, pmd_idx);
    return create_page(pte, pte_idx);
}

/*
 *  Fork page descriptor iteratively
 */

void fork_pte(uint64_t* target_pte, uint64_t* dest_pte) {
    for (int i = 0; i < 512; i++) {
        if (target_pte[i]) {
            uint64_t* target_page = (uint64_t*)((target_pte[i] & PAGE_MASK) | KERNEL_VIRT_BASE);
            uint64_t* dest_page = create_page(dest_pte, i);
            memcpy((void*)dest_page, (void*)target_page, PAGE_SIZE);
        }
    }
}

void fork_pmd(uint64_t* target_pmd, uint64_t* dest_pmd) {
    for (int i = 0; i < 512; i++) {
        if (target_pmd[i]) {
            uint64_t* target_pte = (uint64_t*)((target_pmd[i] & PAGE_MASK) | KERNEL_VIRT_BASE);
            uint64_t* dest_pte = create_page_table(dest_pmd, i);
            fork_pte(target_pte, dest_pte);
        }
    }
}

void fork_pud(uint64_t* target_pud, uint64_t* dest_pud) {
    for (int i = 0; i < 512; i++) {
        if (target_pud[i]) {
            uint64_t* target_pmd = (uint64_t*)((target_pud[i] & PAGE_MASK) | KERNEL_VIRT_BASE);
            uint64_t* dest_pmd = create_page_table(dest_pud, i);
            fork_pmd(target_pmd, dest_pmd);
        }
    }
}

void fork_pgd(struct task_t* target, struct task_t* dest) {
    uint64_t* target_pgd = (uint64_t*)((target->mm.pgd & PAGE_MASK) | KERNEL_VIRT_BASE);
    uint64_t* dest_pgd = create_pgd(dest);
    for (int i = 0; i < 512; i++) {
        if (target_pgd[i]) {
            uint64_t* target_pud = (uint64_t*)((target_pgd[i] & PAGE_MASK) | KERNEL_VIRT_BASE);
            uint64_t* dest_pud = create_page_table(dest_pgd, i);
            fork_pud(target_pud, dest_pud);
        }
    }
}

/*
 *  Page reclaim related function 
 *  Need to be called before exit
 */

void page_free(void* page_virt_addr) {
    uint64_t pfn = phy_to_pfn(virtual_to_physical((uint64_t)page_virt_addr));
    page[pfn].used = AVAL;
    remain_page++;
}

void page_reclaim_pte(uint64_t pte_phy) {
    uint64_t* pte = (uint64_t*)(pte_phy | KERNEL_VIRT_BASE);
    for (int i = 0; i < 512; i++) {
        if (pte[i]) {
            void* page = (void*)((pte[i] & PAGE_MASK) | KERNEL_VIRT_BASE);
            page_free(page);
        }
    }
    page_free(pte);
}

void page_reclaim_pmd(uint64_t pmd_phy) {
    uint64_t* pmd = (uint64_t*)(pmd_phy | KERNEL_VIRT_BASE);
    for (int i = 0; i < 512; i++) {
        if (pmd[i]) {
            page_reclaim_pte(pmd[i] & PAGE_MASK);
        }
    }
    page_free(pmd);
}

void page_reclaim_pud(uint64_t pud_phy) {
    uint64_t* pud = (uint64_t*)(pud_phy | KERNEL_VIRT_BASE);
    for (int i = 0; i < 512; i++) {
        if (pud[i]) {
            page_reclaim_pmd(pud[i] & PAGE_MASK);
        }
    }
    page_free(pud);
}

void page_reclaim(uint64_t pgd_phy) {
    uint64_t* pgd = (uint64_t*)(pgd_phy | KERNEL_VIRT_BASE);
    for (int i = 0; i < 512; i++) {
        if (pgd[i]) {
            page_reclaim_pud(pgd[i] & PAGE_MASK);
        }
    }
    page_free(pgd);
}

/*
 * Utility function
 */

uint64_t user_addr_to_page_addr(uint64_t user_addr, uint64_t pgd_phy) {
    uint64_t pgd_idx = (user_addr & (PD_MASK << PGD_SHIFT)) >> PGD_SHIFT;
    uint64_t pud_idx = (user_addr & (PD_MASK << PUD_SHIFT)) >> PUD_SHIFT;
    uint64_t pmd_idx = (user_addr & (PD_MASK << PMD_SHIFT)) >> PMD_SHIFT;
    uint64_t pte_idx = (user_addr & (PD_MASK << PTE_SHIFT)) >> PTE_SHIFT;

    uint64_t* pgd = (uint64_t*)(pgd_phy | KERNEL_VIRT_BASE);
    uint64_t* pud = (uint64_t*)((pgd[pgd_idx] & PAGE_MASK) | KERNEL_VIRT_BASE);
    uint64_t* pmd = (uint64_t*)((pud[pud_idx] & PAGE_MASK) | KERNEL_VIRT_BASE);
    uint64_t* pte = (uint64_t*)((pmd[pmd_idx] & PAGE_MASK) | KERNEL_VIRT_BASE);
    return (pte[pte_idx] & PAGE_MASK) | KERNEL_VIRT_BASE;
}

uint64_t virtual_to_physical(uint64_t virt_addr) {
    return (virt_addr << 16) >> 16;
}

uint64_t phy_to_pfn(uint64_t phy_addr) {
    return phy_addr >> 12;
}

void memzero(uint8_t* addr, int size) {
    for (int i = 0; i < size; i++) {
        *(addr + i) = 0;
    }
}

void memcpy(void* dest, void* src, uint64_t size) {
    uint8_t* csrc = (uint8_t*)src;
    uint8_t* cdest = (uint8_t*)dest;

    for (uint64_t i = 0; i < size; i++)
        cdest[i] = csrc[i];
}