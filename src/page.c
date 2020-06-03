#include "../include/mm.h"
#include "../include/mmu.h"
#include "../include/task.h"
#include "../include/page.h"

struct page_manager PageManager;


// struct page* translation(unsigned long kvirtual_address)
// {
//     return &PageManager.page_pool[(kvirtual_address >> 12) & ((1<<36)-1)];
// }

void init_page()
{	
	PageManager.remain_page = LAST_AVAIL_PAGE;
	for (int i=0; i<FIRST_AVAIL_PAGE; i++) {
		PageManager.pages[i].state = PRESERVE;
		PageManager.remain_page--;
	}
	for (int i=FIRST_AVAIL_PAGE; i<LAST_AVAIL_PAGE; i++) {
		PageManager.pages[i].state = NOT_USED;
	}
}

unsigned long va_to_pa(unsigned long va)
{
	unsigned long pfn = (va << 16) >> 16;
	unsigned long offset = (va << 52) >> 52;
	pfn = pfn >> PTE_SHIFT;

	return (pfn * PAGE_SIZE) | offset;
}

unsigned long pa_to_pfn(unsigned long pa){
	return pa >> 12;	
}

unsigned long allocate_kernel_page() 
{
	unsigned long page = get_free_page();
	printf("page at %x\n", page);
	if (page == 0) {
		return 0;
	}
	return page + VA_START;
}

unsigned long get_free_page()
{
	// printf("PAGING_PAGES: %x\n", PAGING_PAGES);
	for (int i = FIRST_AVAIL_PAGE; i < LAST_AVAIL_PAGE; i++){
		if (PageManager.pages[i].state == NOT_USED){
			PageManager.pages[i].state = IN_USED;
			PageManager.remain_page--;
			memzero((unsigned long) (i * PAGE_SIZE) + VA_START, PAGE_SIZE);
			return i * PAGE_SIZE;
		}
	}
	return 0;
}

void free_page(unsigned long p)
{
	int pfn = (p - LOW_MEMORY) / PAGE_SIZE;
	if (PageManager.pages[pfn].state == IN_USED)
	{
		PageManager.pages[pfn].state = NOT_USED;
	}
	PageManager.remain_page++; 
}


unsigned long allocate_user_page(struct task *task, unsigned long va)
{
	unsigned long page = get_free_page(); 
	if(page == 0){
		return 0;
	}
	// printf("task->mm.pgd: %x\n", task->mm.pgd);
	map_page(task, va, page, MMU_PTE_FLAGS); // maps it to the provided virtual address
	// printf("task->mm.pgd: %x\n", task->mm.pgd);
	return page + VA_START;
}

void map_page(struct task *task, unsigned long va, unsigned long page, unsigned long page_attr)
{
	unsigned long pgd;
	
	// If it is the first time to map this task
	if(!task->mm.pgd){
		task->mm.pgd = get_free_page();
		task->mm.kernel_pages[task->mm.kernel_pages_count++] = task->mm.pgd;
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
	
	// last table will be pte table
	map_entry((unsigned long *)(pte + VA_START), va, page, page_attr);

	struct user_page p = {page, (va>>12)<<12};
	// struct user_page p = {page, va};
	task->mm.user_pages[task->mm.user_pages_count++] = p;
}

unsigned long map_table(unsigned long *table, unsigned long shift, unsigned long va, struct task *task) 
{   	
	unsigned long index = va >> shift;
	index = index & (PTRS_PER_TABLE - 1);
	if(!table[index]){	
		unsigned long next_level_table = get_free_page();
		unsigned long entry = next_level_table | MM_TYPE_PAGE_TABLE;
		table[index] = entry;
		task->mm.kernel_pages[task->mm.kernel_pages_count++] = next_level_table;
		return next_level_table;
	}
	else{  // case that child table was already allocated 
		return (table[index]>>12)<<12;
	}
}

void map_entry(unsigned long *pte, unsigned long va, unsigned long pa, unsigned long page_attr) 
{
    unsigned long index = va >> 12;
    index = index & (PTRS_PER_TABLE - 1);
    unsigned long entry = pa | page_attr;
    pte[index] = entry;
}

// void* mmap(void* addr, unsigned long len, int prot, int flags, void* file_start, int file_offset)
// {
// 	unsigned long va;
// 	if(addr!=NULL)
// 		va = ((unsigned long)(addr)>>12)<<12;
// 	else{
// 		va = 0x2000;
// 	}
// }

// void page_fault_handler()
// {
// 	unsigned long far_el1, esr_el1;
// 	asm volatile ("mrs %0, far_el1" : "=r"(far_el1));
// 	asm volatile ("mrs %0, esr_el1" : "=r"(esr_el1));
// }

// int page_fault_handler(unsigned long addr,unsigned long esr){
//         //printf("+++ Page faalt at 0x%x\r\n",addr);
// 	if(((esr>>2)&0x3) != 1){ //If not a translation fault, kill  
//   		 switch((esr>>2)&0x3) {
//  			  case 0: uart_send_string("Address size fault at"); break;
//                           case 2: uart_send_string("Access flag fault at"); break;
//                           case 3: uart_send_string("Permission fault at"); break;
//                   }
 
//                  printf("### Data abort at 0x%x, killed\r\n",addr);
//                  exit_process();
//                  return -1;
//          }

	

// 	// If not a map region, kill
// 	printf("### Page fault address at 0x%x, killed\r\n",addr);
// 	exit_process(); 	
// 	return -1;
		
// }