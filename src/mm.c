#include "mm.h"
#include "mmu.h"

#include "peripherals/mmio.h"
#include "schedule.h"
#include "uart0.h"

struct page_t page[PAGE_FRAMES_NUM];
int first_aval_page, last_aval_page;
uint64_t remain_page = 0;

void mm_init() {
    extern uint8_t __kernel_end;  // defined in linker
    uint8_t* kernel_end = &__kernel_end;
    int kernel_end_page = (uint64_t)kernel_end / PAGE_SIZE;
    int mmio_base_page = MMIO_PHYSICAL / PAGE_SIZE;

    int i = 0;
    for (; i < kernel_end_page; i++) {
        page[i].used = USED;
    }
    for (; i < mmio_base_page; i++) {
        remain_page++;
        page[i].used = AVAL;
    }
    for (; i < PAGE_FRAMES_NUM; i++) {
        page[i].used = USED;
    }

    first_aval_page = kernel_end_page + 1;
    last_aval_page = mmio_base_page - 1;
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
    current_task->mm.kernel_pages[current_task->mm.kernel_pages_count++] = page_virt_addr;
    remain_page--;
    return (void*)page_virt_addr;
}

// get one page for user space
void* page_alloc_user() {
    uint64_t page_phy_addr = get_free_page();
    if (page_phy_addr == 0) {
        return NULL;
    }
    uint64_t page_virt_addr = page_phy_addr | KERNEL_VIRT_BASE;
    current_task->mm.user_pages[current_task->mm.user_pages_count++] = page_virt_addr;
    remain_page--;
    return (void*)page_virt_addr;
}

void page_free(void* virt_addr) {
    uint64_t pfn = phy_to_pfn(virtual_to_physical((uint64_t)virt_addr));
    page[pfn].used = AVAL;
    remain_page++;
}

// create pgd, return pgd address
void* create_pgd() {
    struct task_t* current = get_current_task();
    if (!current->mm.pgd) {
        void* page = page_alloc();
        if (page == NULL) return NULL;
        current->mm.pgd = virtual_to_physical((uint64_t)page);
    }
    return (void*)(current->mm.pgd + KERNEL_VIRT_BASE);
}

// create page table, return next level table
void* create_page_table(uint64_t *table, uint64_t idx) {
    if (table == NULL) return NULL;
    if (!table[idx]) {
        void* page = page_alloc();
        if (page == NULL) return NULL;
        table[idx] = virtual_to_physical((uint64_t)page) | PD_TABLE;
    }
    return (void*)((table[idx] & ~0xFFF) + KERNEL_VIRT_BASE);
}

// create page, return page address
void* create_page(uint64_t *pte, uint64_t idx) {
    if (pte == NULL) return NULL;
    if (!pte[idx]) {
        void* page = page_alloc_user();
        if (page == NULL) return NULL;
        pte[idx] = virtual_to_physical((uint64_t)page) | PTE_NORAL_ATTR | PD_ACCESS_PERM_RW;
    }
    return (void*)((pte[idx] & ~0xFFF) + KERNEL_VIRT_BASE);
}

// allocate new page in user's address table, return page's virtual address
void* map_addr_user(uint64_t user_addr) {
    uint64_t pgd_idx = (user_addr & (PD_MASK << PGD_SHIFT)) >> PGD_SHIFT;
    uint64_t pud_idx = (user_addr & (PD_MASK << PUD_SHIFT)) >> PUD_SHIFT;
    uint64_t pmd_idx = (user_addr & (PD_MASK << PMD_SHIFT)) >> PMD_SHIFT;
    uint64_t pte_idx = (user_addr & (PD_MASK << PTE_SHIFT)) >> PTE_SHIFT;

    uint64_t* pgd = create_pgd();
    uint64_t* pud = create_page_table(pgd, pgd_idx);
    uint64_t* pmd = create_page_table(pud, pud_idx);
    uint64_t* pte = create_page_table(pmd, pmd_idx);
    return create_page(pte, pte_idx);
}
