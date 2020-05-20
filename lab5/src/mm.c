#include "../include/mm.h"
#include "arm/mmu.h"
#include "printf.h"
static unsigned short mem_map [ PAGING_PAGES ] = {0,};
static unsigned num_free_pages = PAGING_PAGES ;

unsigned long allocate_kernel_page() {
	unsigned long page = get_free_page();
	if (page == 0) {
		return 0;
	}
	return page + VA_START;
}

unsigned long allocate_user_page(struct task_struct *task, unsigned long va, unsigned long prot) {
	unsigned long page = get_free_page();
	if (page == 0) {
		return 0;
	}
	map_page(task, va, page, prot);
	return page + VA_START;
}

unsigned long get_free_page()
{
	for (int i = 0; i < PAGING_PAGES; i++){
		if (mem_map[i] == 0){
			mem_map[i] = 1;
			unsigned long page = LOW_MEMORY + i*PAGE_SIZE;
			memzero(page + VA_START, PAGE_SIZE);
			num_free_pages -= 1;
			return page;
		}
	}
	return 0;
}

void free_page(unsigned long p){  // free page input is physical memory
	mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
	return;
}

void free_user_page(struct task_struct *task) {
	for (int i = 0 ; i < task->mm.user_pages_count ; i++) {
		free_page(task->mm.user_pages[i].phys_addr);
		num_free_pages += 1;
	}
	return;
}

void free_kernel_page(struct task_struct *task) {
	for (int i = 1 ; i <= task->mm.kernel_pages_count ; i++) {
		free_page(task->mm.kernel_pages[i]);
		num_free_pages += 1;
	}
	return;
}

void map_table_entry(unsigned long *pte, unsigned long va, unsigned long pa, unsigned long flag) {
	unsigned long index = va >> PAGE_SHIFT;
	index = index & (PTRS_PER_TABLE - 1);
	unsigned long entry = pa | flag; // put protection flag
	pte[index] = entry;
}

unsigned long map_table(unsigned long *table, unsigned long shift, unsigned long va, int* new_table) {
	unsigned long index = va >> shift; // find the index of page table
	index = index & (PTRS_PER_TABLE - 1); // mask the attributes
	if (!table[index]){ 
		*new_table = 1;
		unsigned long next_level_table = get_free_page();
		unsigned long entry = next_level_table | MM_TYPE_PAGE_TABLE; //entry is physical address | MM_TYPE 
		table[index] = entry;
		return next_level_table;
	} else {
		*new_table = 0;
	}
	return table[index] & PAGE_MASK;
}

void map_page(struct task_struct *task, unsigned long va, unsigned long page, unsigned long prot){
	unsigned long pgd;
	if (!task->mm.pgd) {
		task->mm.pgd = get_free_page(); // allocate a page and return physical address
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = task->mm.pgd;
	}
	pgd = task->mm.pgd;
	int new_table;
	unsigned long pud = map_table((unsigned long *)(pgd + VA_START), PGD_SHIFT, va, &new_table);
	if (new_table) {
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = pud; // store pud table physical address
	}
	unsigned long pmd = map_table((unsigned long *)(pud + VA_START) , PUD_SHIFT, va, &new_table);
	if (new_table) {
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = pmd;
	}
	unsigned long pte = map_table((unsigned long *)(pmd + VA_START), PMD_SHIFT, va, &new_table);
	if (new_table) {
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = pte;
	}
	map_table_entry((unsigned long *)(pte + VA_START), va, page, prot);
	struct user_page p = {page, va};
	task->mm.user_pages[task->mm.user_pages_count++] = p;
}

int copy_virt_memory(struct task_struct *dst) {
	struct task_struct* src = current;
	for (int i = 0; i < src->mm.user_pages_count; i++) {
		unsigned long kernel_va = allocate_user_page(dst, src->mm.user_pages[i].virt_addr, MMU_PTE_FLAGS);
		if( kernel_va == 0) {
			return -1;
		}
		memcpy(src->mm.user_pages[i].virt_addr, kernel_va, PAGE_SIZE);
	}
	return 0;
}

static int ind = 1;

int do_mem_abort(unsigned long addr, unsigned long esr) {
	unsigned long dfs = (esr & 0b111111);
	if ((dfs & 0b111100) == 0b100) {
		unsigned long page = get_free_page();
		if (page == 0) {
			return -1;
		}
		map_page(current, addr & PAGE_MASK, page, MMU_PTE_FLAGS);
		ind++;
		if (ind > 2){
			unsigned long val;
			printf("esr: 0x%x\r\n", dfs);
			asm volatile ("mrs %0, far_el1" : "=r" (val));
			printf("page fault address: 0x%x\r\n", val);
			exit_process();
			return -1;
		}
		return 0;
	}
	else {
		printf("page fault\r\n");
		exit_process();
	}
	return -1;
}

int get_remain_num() {
	return num_free_pages;
}

void free_zombie_task() {
	for (int i = 0 ; i < nr_tasks ; i++) {
		if (task[i]->state == TASK_ZOMBIE) {
			free_kernel_page((unsigned long)task[i]);
			task[i]->state = TASK_FREE;
			free_page(((unsigned long)task[i]) - VA_START);
			num_free_pages += 1;
			
		}
	}
} 


unsigned long do_mmap(unsigned long addr, struct task_struct * task, unsigned long len, unsigned long prot) 
{
	unsigned long begin_addr;
	unsigned long last_page;

	if (addr == 0) {
		last_page = task->mm.user_pages[task->mm.user_pages_count - 1].virt_addr; //last virtual address
		begin_addr = last_page + PAGE_SIZE;
		last_page += PAGE_SIZE;
	}
	else {
		last_page  = addr & PAGE_MASK;
		begin_addr = addr & PAGE_MASK;
	}
	int page_num = (len -1) / PAGE_SIZE + 1; //WARN len = 0

	for (int i = 0 ; i < page_num ; i++) {
		unsigned long page = get_free_page();
		if (page == 0) {
			return;
		}
		map_page(task, last_page, page, prot); // map for user task
		last_page += PAGE_SIZE;
	}
	return begin_addr;
}