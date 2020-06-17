#include "mm.h"

#define NOTUSED 0
#define USED    1

static unsigned short mem_map [ PAGING_PAGES ] = {0,};

unsigned long allocate_kernel_page() {
    unsigned long page = get_free_page();
    if (page == 0) {
        return 0;
    }
    return page + VA_START;
}

unsigned long allocate_user_page(struct task *task, unsigned long va) {
    unsigned long page = get_free_page();
    if (page == 0) {
        return 0;
    }
    map_page(task, va, page);
    return page + VA_START;
}

void map_page(struct task *task, unsigned long va, unsigned long page){
    unsigned long pgd;
    if (!task->mm.pgd) {
        task->mm.pgd = get_free_page();
        task->mm.kernel_pages[++task->mm.kernel_pages_count] = task->mm.pgd;
    }
    pgd = task->mm.pgd;
    int new_table;
    unsigned long pud = map_table((unsigned long *)(pgd + VA_START), PGD_SHIFT, va, &new_table);
    if (new_table) {
        task->mm.kernel_pages[++task->mm.kernel_pages_count] = pud;
    }
    unsigned long pmd = map_table((unsigned long *)(pud + VA_START) , PUD_SHIFT, va, &new_table);
    if (new_table) {
        task->mm.kernel_pages[++task->mm.kernel_pages_count] = pmd;
    }
    unsigned long pte = map_table((unsigned long *)(pmd + VA_START), PMD_SHIFT, va, &new_table);
    if (new_table) {
        task->mm.kernel_pages[++task->mm.kernel_pages_count] = pte;
    }
    
    map_table_entry((unsigned long *)(pte + VA_START), va, page);
    struct user_page p = {page, va};
    task->mm.user_pages[task->mm.user_pages_count++] = p;
}

void map_table_entry(unsigned long *pte, unsigned long va, unsigned long pa) {
    unsigned long index = va >> PAGE_SHIFT;
    index = index & (PTRS_PER_TABLE - 1);
    unsigned long entry = pa | MMU_PTE_FLAGS; 
    pte[index] = entry;
}

unsigned long map_table(unsigned long *table, unsigned long shift, unsigned long va, int* new_table) {
    unsigned long index = va >> shift;
    index = index & (PTRS_PER_TABLE - 1);
    if (!table[index]){
        *new_table = 1;
        unsigned long next_level_table = get_free_page();
        unsigned long entry = next_level_table | MM_TYPE_PAGE_TABLE;
        table[index] = entry;
        return next_level_table;
    } else {
        *new_table = 0;
    }
    return table[index] & PAGE_MASK;
}

unsigned long get_free_page()
{
    for (int i = INITUSED; i < PAGING_PAGES; i++){
        if (pages[i].used == NOTUSED) { 
            pages[i].used  = USED;
            unsigned long page =  i * PAGE_SIZE;
            memzero(page + VA_START, PAGE_SIZE);
            return page;
        }
    }
    return 0;
}

void free_page(unsigned long p){
    mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}

void fork_memcpy (void *dest, const void *src, unsigned long len)
{
    char *d = dest;
    const char *s = src;
    while (len--)
            *d++ = *s++;
}

void init_pages(){
    int i;
    for(i = 0; i < INITUSED; i++){
        pages[i].used = USED;
    }

    for(;i < NUMPAGES; i++){
        pages[i].used = NOTUSED;
    }
    
}

unsigned long virtual_to_physical(unsigned long vir){
    unsigned long delffff = (vir << 16) >> 16;
    unsigned long pfn = delffff >> 12;
    unsigned long offset = (vir << 52) >> 52;
    return (pfn * PAGE_SIZE) | offset;;
}

unsigned long physical_to_pfn(unsigned long phy){
    return phy >> 12;
}

