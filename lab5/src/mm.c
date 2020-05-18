#include "io.h"
#include "mm.h"
#include "task.h"
#include "armmmu.h"

extern Task *current_task;
#undef PAGING_PAGES
#define PAGING_PAGES (1024 * 64)
Page mpages[PAGING_PAGES] = {[0 ... PAGING_PAGES - 1] = {empty} };

unsigned long vir2phy(unsigned long vir){

  //vir &= (~VA_START);
  //unsigned long idx = (vir - LOW_MEMORY) / PAGE_SIZE;
	unsigned long pfn = (vir << 16) >> 16;
	unsigned long offset = (vir << 52) >> 52;
	pfn = (pfn)>>PAGE_SHIFT;
	return pfn * PAGE_SIZE | offset;
}

unsigned long phy2pfn(unsigned long phy){
	return phy >> 12;
}

void mark_reserved_pages(unsigned long end){
  // most free because LOW_MEMORY HIGH_MEMORY protect
  printf("LOW_MEMORY: 0x%x " NEWLINE
      "HIGH_MEMORY: 0x%x" NEWLINE,
      LOW_MEMORY, HIGH_MEMORY);
  for (int i = 0; i < PAGING_PAGES; i++){
			unsigned long addr = LOW_MEMORY + i*PAGE_SIZE + VA_START;
      if(addr <= end || addr >= VA_START + HIGH_MEMORY){
        mpages[i].status = reserved;
        printf("page[%d] %x reserved." NEWLINE, i, addr);
      }
      //else printf("page[%d] %x empty." NEWLINE, i, addr);
	}
}

unsigned long get_free_page()
{
	for (int i = 0; i < PAGING_PAGES; i++){
		if (mpages[i].status == empty){
			mpages[i].status = used;
			unsigned long page = LOW_MEMORY + i * PAGE_SIZE;
			memzero(page + VA_START, PAGE_SIZE);
      printf("free page 0x%x" NEWLINE, page);
			return page;
		}
	}
  printf("free page failed." NEWLINE);
	return 0;
}

unsigned long allocate_kernel_page() {
	unsigned long page = get_free_page();
	if (page == 0) {
		return 0;
	}
	return page + VA_START;
}

unsigned long allocate_user_page(Task *task, unsigned long va) {
	unsigned long page = get_free_page();
	if (page == 0) {
		return 0;
	}
	map_page(task, va, page);
	return page + VA_START;
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

void map_table_entry(unsigned long *pte, unsigned long va, unsigned long pa) {
	unsigned long index = va >> PAGE_SHIFT;
	index = index & (PTRS_PER_TABLE - 1);
	unsigned long entry = pa | MMU_PTE_FLAGS;
	pte[index] = entry;
}

void map_page(Task *task, unsigned long va, unsigned long page){
	unsigned long pgd;
	if (!task->mm.pgd) {
		task->mm.pgd = get_free_page();
		task->mm.kernel_pages[task->mm.kernel_pages_count++] = task->mm.pgd;
	}
	pgd = task->mm.pgd;
	int new_table;
	unsigned long pud = map_table((unsigned long *)(pgd + VA_START), PGD_SHIFT, va, &new_table);
	if (new_table) {
		task->mm.kernel_pages[task->mm.kernel_pages_count++] = pud;
	}
	unsigned long pmd = map_table((unsigned long *)(pud + VA_START) , PUD_SHIFT, va, &new_table);
	if (new_table) {
		task->mm.kernel_pages[task->mm.kernel_pages_count++] = pmd;
	}
	unsigned long pte = map_table((unsigned long *)(pmd + VA_START), PMD_SHIFT, va, &new_table);
	if (new_table) {
		task->mm.kernel_pages[task->mm.kernel_pages_count++] = pte;
	}
	map_table_entry((unsigned long *)(pte + VA_START), va, page);
	struct user_page p = {page, va};
	task->mm.user_pages[task->mm.user_pages_count++] = p;
}

void free_page(unsigned long p){
	mpages[(p - LOW_MEMORY) / PAGE_SIZE].status = empty;
}


int copy_virt_memory(Task *dst) {
	Task* src = current_task;
	for (int i = 0; i < src->mm.user_pages_count; i++) {
		unsigned long kernel_va = allocate_user_page(dst, src->mm.user_pages[i].virt_addr);
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
		map_page(current_task, addr & PAGE_MASK, page);
		ind++;
		if (ind > 2){
			return -1;
		}
		return 0;
	}
	return -1;
}
