#include "mm.h"
#include "mmu.h"

#include "peripherals/mmio.h"
#include "schedule.h"
#include "uart0.h"

struct page_t page[PAGE_FRAMES_NUM];
int first_aval_page, last_aval_page;

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

void* page_alloc() {
    uint64_t page_phy_addr = get_free_page();
    if (page_phy_addr == 0) {
        return NULL;
    }
    return (void*)(page_phy_addr + KERNEL_VIRT_BASE);
}

void page_free(void* virt_addr) {
    uint64_t pfn = phy_to_pfn(virtual_to_physical((uint64_t)virt_addr));
    page[pfn].used = AVAL;
}

void* page_alloc_user(struct task_t* task, uint64_t user_virt_addr) {  // map one page to user_virt_addr
    /*
     * Allocate new page in user's address table
     */
    uint64_t pgd_idx = (user_virt_addr & (PD_MASK << PGD_SHIFT)) >> PGD_SHIFT;
    uint64_t pud_idx = (user_virt_addr & (PD_MASK << PUD_SHIFT)) >> PUD_SHIFT;
    uint64_t pmd_idx = (user_virt_addr & (PD_MASK << PMD_SHIFT)) >> PMD_SHIFT;
    uint64_t pte_idx = (user_virt_addr & (PD_MASK << PTE_SHIFT)) >> PTE_SHIFT;

    // create PGD
    if (!task->mm.pgd) {
        uint64_t page_phy_addr = get_free_page();
        if (page_phy_addr == 0) {
            return NULL;
        }
        task->mm.pgd = page_phy_addr;
    }
    uint64_t* pgd = (uint64_t*)(task->mm.pgd + KERNEL_VIRT_BASE);
    // create PUD
    if (!pgd[pgd_idx]) {
        uint64_t page_phy_addr = get_free_page();
        if (page_phy_addr == 0) {
            return NULL;
        }
        pgd[pgd_idx] = page_phy_addr | PD_TABLE;
    }
    uint64_t* pud = (uint64_t*)((pgd[pgd_idx] & ~0xFFF) + KERNEL_VIRT_BASE);
    // create PMD
    if (!pud[pud_idx]) {
        uint64_t page_phy_addr = get_free_page();
        if (page_phy_addr == 0) {
            return NULL;
        }
        pud[pud_idx] = page_phy_addr | PD_TABLE;
    }
    uint64_t* pmd = (uint64_t*)((pud[pud_idx] & ~0xFFF) + KERNEL_VIRT_BASE);
    // create PTE
    if (!pmd[pmd_idx]) {
        uint64_t page_phy_addr = get_free_page();
        if (page_phy_addr == 0) {
            return NULL;
        }
        pmd[pmd_idx] = page_phy_addr | PD_TABLE;
    }
    uint64_t* pte = (uint64_t*)((pmd[pmd_idx] & ~0xFFF) + KERNEL_VIRT_BASE);
    // allocate page for virtual address
    if (!pte[pte_idx]) {
        uint64_t page_phy_addr = get_free_page();
        if (page_phy_addr == 0) {
            return NULL;
        }
        pte[pte_idx] = (page_phy_addr | (PD_ACCESS_FLAG | PD_ACCESS_PERM_RW | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_PAGE)) & ~PD_EXN;
    }

    return (void*)((pte[pte_idx] & ~0xFFF) + KERNEL_VIRT_BASE);
}
