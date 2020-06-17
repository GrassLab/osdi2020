#include "mm.h"
#include "sched.h"
#include "arm/sysregs.h"

unsigned short memory_map[NUM_PAGES] = {0,};
struct page_struct page[LAST_PAGE];

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

unsigned long vaddr_to_paddr(unsigned long vaddr) {
    vaddr = vaddr & 0x0000fffffffff000;
    return vaddr;
}

unsigned long get_pfn(unsigned long addr) {
    addr = addr & 0x0000fffffffff000;
    addr >>= 12;
    return addr;
}

unsigned long map_table(unsigned long* table, unsigned long shift,
               unsigned long virt_addr, struct task_struct *task) {
    unsigned long index = virt_addr >> shift;
    index = (index & TABLE_MASK);
    
    if (!table[index]) {
        unsigned long next_level_table = get_free_page();
        unsigned long entry = next_level_table | PD_TABLE;
        table[index] = entry;
        task->mm.kernel_pages[task->mm.kernel_pages_count ++] = next_level_table;
        return next_level_table;
    } else {
        return (table[index] & 0xfffffffffffff000);
    }
}

void map_entry(unsigned long *pte, unsigned long virt_addr,
               unsigned long phy_addr, unsigned long page_attr) {
    unsigned long index = virt_addr >> 12;
    index = (index & TABLE_MASK);
    unsigned long entry = phy_addr | page_attr;
    pte[index] = entry;
}

void map_page(struct task_struct *task, unsigned long virt_addr,
              unsigned long page, unsigned long page_attr) {
    unsigned long pgd;
    
    if (!task->mm.pgd) {
        // allocate a page for placeing the user pgd
        task->mm.pgd = get_free_page();
        task->mm.kernel_pages[task->mm.kernel_pages_count++] = task->mm.pgd;
    }
    pgd = task->mm.pgd;

    unsigned long table = pgd;
    int shift = 12 + 9 + 9 + 9;
    for (int i = 0; i < 3; ++ i) {
        table = map_table((unsigned long*)(table + VA_START), shift, 
                          virt_addr, task);
        shift -= 9;
    }
    map_entry((unsigned long *)(table + VA_START), virt_addr, page, page_attr);

    struct user_page p = {page, (virt_addr & 0xfffffffffffff000)};
    task->mm.user_pages[task->mm.user_pages_count++] = p;
}

/* 0 means allocate failed, caller should check */
unsigned long get_free_page() {
    unsigned long addr = 0;
    for (int i = FIRST_AVAIL_PAGE; i < LAST_PAGE; ++ i) {
        if (page[i].in_use)
            continue;
        page[i].in_use = 1;
        addr = i * PAGE_SIZE;
        memzero(addr, PAGE_SIZE);
    }
    return addr;
}

void page_free(unsigned long addr) {
    page[get_pfn(addr)].in_use = 0;
}

void init_page_status() {
    for (int i = 0; i < FIRST_AVAIL_PAGE; ++ i) {
        page[i].in_use = 1;
    }
}
