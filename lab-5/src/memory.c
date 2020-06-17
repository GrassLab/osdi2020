#include "memory.h"
#include "register.h"
#include "task.h"

int remain_page = PAGE_ENTRY;
Page pages[PAGE_ENTRY];

void map_page(struct task *task, unsigned long va, unsigned long page, unsigned long page_attr);
unsigned long map_table(unsigned long *table, unsigned long shift, unsigned long va, struct task *task);
void map_entry(unsigned long *pte, unsigned long va, unsigned long pa, unsigned long page_attr);

void paging_init() {
	memset((unsigned long) pages, 0, sizeof(Page) * PAGE_ENTRY);

	unsigned long pfn;	
	pfn = virtual_to_pfn(LOW_MEMORY);
	
	for(int i = 0; i < pfn; i++){
		pages[i].used = PAGE_PRESERVE; 	
	}
}

unsigned long get_free_page() {
	// Start from first availible memory
	// Since some region are used for kernel image / stack
	for (int i = FIRST_AVAILIBLE_PAGE; i < PAGE_ENTRY; i++){
		if (pages[i].used == PAGE_NOT_USED){
			pages[i].used = PAGE_USED;

			unsigned long page = i * PAGE_SIZE;
			memset(VA_START + page, 0, PAGE_SIZE);
			return page;
		}
	}
	return 0;
}

unsigned long allocate_kernel_page() {
	unsigned long page = get_free_page();
	if (page == 0) {
		return 0;
	}
	return page + VA_START;
}

unsigned long allocate_user_page(struct task *task, unsigned long va) {
	unsigned long page = get_free_page(); 
	if(page == 0){
		return 0;
	}
	map_page(task, va, page, MMU_PTE_FLAGS); // maps it to the provided virtual address
	return page + VA_START;
}

void free_page(unsigned long p) {
	if(pages[p / PAGE_SIZE].used == PAGE_USED)
		pages[p / PAGE_SIZE].used = PAGE_NOT_USED;
}

unsigned long virtual_to_physical(unsigned long vir) {
	unsigned long pfn = (vir << 16) >> 16;
	unsigned long offset = (vir << 52) >> 52;
	pfn = (pfn) >> PTE_SHIFT;
	return pfn*PAGE_SIZE | offset;
}

unsigned long virtual_to_pfn(unsigned long vir){
	// using [47:21] being fpn for 2M section
	unsigned long pfn = (vir<<16)>>16;
	pfn = (pfn)>>PTE_SHIFT;
	return pfn;	
}

unsigned long physical_to_pfn(unsigned long phy){
	return (phy) >> 12;	
}

void map_page(struct task *task, unsigned long va, unsigned long page, unsigned long page_attr)
{
	unsigned long pgd;
	
	// If it is the first time to map this task
	if(!task->pageInfo.pgd){
		task->pageInfo.pgd = get_free_page();
		task->pageInfo.kernelPages[task->pageInfo.kernelPageCount++] = task->pageInfo.pgd;
	}
	
	pgd = task->pageInfo.pgd;

	int new_table;
    unsigned long pud = map_table((unsigned long *)(pgd + VA_START), PGD_SHIFT, va, &new_table);
    if (new_table) {
        task->pageInfo.kernelPages[++task->pageInfo.kernelPageCount] = pud;
    }
    unsigned long pmd = map_table((unsigned long *)(pud + VA_START) , PUD_SHIFT, va, &new_table);
    if (new_table) {
        task->pageInfo.kernelPages[++task->pageInfo.kernelPageCount] = pmd;
    }
    unsigned long pte = map_table((unsigned long *)(pmd + VA_START), PMD_SHIFT, va, &new_table);
    if (new_table) {
        task->pageInfo.kernelPages[++task->pageInfo.kernelPageCount] = pte;
    }
	
	// last table will be pte table
	map_entry((unsigned long *)(pte + VA_START), va, page, page_attr);

	Page p = {page, (va >> 12) << 12};
	// struct user_page p = {page, va};
	task->pageInfo.userPages[task->pageInfo.userPageCount++] = p;
}

unsigned long map_table(unsigned long *table, unsigned long shift, unsigned long va, Task *task)
{   	
	unsigned long index = va >> shift;
	index = index & (PTRS_PER_TABLE - 1);
	if (!table[index]) {	
		unsigned long next_level_table = get_free_page();
		unsigned long entry = next_level_table | MM_TYPE_PAGE_TABLE;
		table[index] = entry;
		task->pageInfo.kernelPages[task->pageInfo.kernelPageCount++] = next_level_table;
		return next_level_table;
	} else {  // case that child table was already allocated 
		return (table[index] >> 12) << 12;
	}
}

void map_entry(unsigned long *pte, unsigned long va, unsigned long pa, unsigned long page_attr) 
{
    unsigned long index = va >> 12;
    index = index & (PTRS_PER_TABLE - 1);
    unsigned long entry = pa | page_attr;
    pte[index] = entry;
}