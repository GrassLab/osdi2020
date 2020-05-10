#include "include/mm.h"
#include "include/printf.h"
#include "include/uart.h"
#include "include/arm/sysreg.h"
#include "include/scheduler.h"

unsigned long get_free_page() // this function can only call by 
	                      // 1.alloc kernel pg
       			      // 2. alloc user pg	
{
	// Start from first availible memory
	// Since some region are used for kernel image / stack
	
	for (int i = FIRST_AVAILIBLE_PAGE; i < PAGE_ENTRY; i++){
		// finding availible memory space for your process
		if (page[i].used == NOT_USED){
			//printf("Using Page: 0x%x\r\n",i*PAGE_SIZE);
			page[i].used = USED_NOW;
			// initialize to zero
			memzero((unsigned long) (i * PAGE_SIZE) + VA_START, PAGE_SIZE);
			return i * PAGE_SIZE;
		}
	}
	return 0;
}

unsigned long allocate_kernel_page(){
	unsigned long page = get_free_page();
	if(page == 0){
		return 0;
	}
	return page + VA_START;
}

unsigned long allocate_user_page(struct task_struct *task, \
		unsigned long vir_addr){

	unsigned long page = get_free_page(); 
	if(page == 0){
		return 0;
	}
	map_page(task,vir_addr,page); // maps it to the provided virtual address
	return page + VA_START;
}

void map_page(struct task_struct *task, unsigned long vir_addr, \
		unsigned long page)
{
	unsigned long pgd;
	
	if(!task->mm.pgd){
		task->mm.pgd = get_free_page();
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = task->mm.pgd;
	}
	pgd = task->mm.pgd;

	unsigned long table = pgd;
	int  shift = 39;
	// setting PUD-> PMD -> PTE table
	for(int i=0;i<3;i++){
		table = map_table((unsigned long *)(table+VA_START),shift,vir_addr,task);
		shift-=9;
	}
	
	// last table will be pte table
	map_entry((unsigned long *)(table+VA_START), vir_addr, page);

	struct user_page p = {page, vir_addr};
	task->mm.user_pages[task->mm.user_pages_count++] = p;
}

unsigned long map_table(unsigned long *table, unsigned long shift, \
	       	unsigned long vir_addr, struct task_struct *task) {
   	
	unsigned long index = vir_addr >> shift;
    	index = index & (PTRS_PER_TABLE - 1);
    	
	if (!table[index]){
        	unsigned long next_level_table = get_free_page();
        	unsigned long entry = next_level_table | PD_TABLE;
        	table[index] = entry;
        	task->mm.kernel_pages[++task->mm.kernel_pages_count] = next_level_table;
		return next_level_table;
    	}
    
	return table[index] & PAGE_MASK;
}

void map_entry(unsigned long *pte, unsigned long vir_addr,\
	       	unsigned long phy_addr) {

    unsigned long index = vir_addr >> 12;
    index = index & (PTRS_PER_TABLE - 1);
    unsigned long entry = phy_addr | MMU_PTE_FLAGS;
    pte[index] = entry;
}

void free_page(unsigned long p){
	//printf("Free Page %d\r\n", p/PAGE_SIZE);	
	if(page[ p / PAGE_SIZE].used==USED_NOW)
		page[ p / PAGE_SIZE].used = NOT_USED;
}

void fork_memcpy (void *dest, const void *src, unsigned long len)
{
  	char *d = dest;
  	const char *s = src;
  	while (len--)
    		*d++ = *s++;
}

void init_page_struct(){	
	for(int i=0;i<FIRST_AVAILIBLE_PAGE;i++){
		page[i].used = PRESERVE;
	}
}

unsigned long virtual_to_physical(unsigned long vir){
	unsigned long pfn = (vir<<16)>>16;
	unsigned long offset = (vir<<52)>>52;
	pfn = (pfn)>>PTE_SHIFT;

	return pfn*PAGE_SIZE | offset;
}

unsigned long physical_to_pfn(unsigned long phy){
	return (phy)>>12;	
}

void dump_mem(void *src,unsigned long len){
         const char *s = src;
	 int count = 0;
         while (len--){
		 if(count==0)
			 printf("0x%x ",s);
		 count++;

		 if(*s<10)
			 printf("0");
                 printf("%x ",*s);
		 if(count>=16){
			 count=0;
			 printf("\r\n");
		 }
                 s++;
         }
	 printf("\r\n");
}

int do_mem_abort(unsigned long addr, unsigned long esr){
	//printf("Fault address at 0x%x%x\r\n",addr>>32,addr);
	if ( (esr&0x7) == 0x7  ) { // translation fault
		unsigned long page = get_free_page();
		if (page == 0) 
			return -1;
		map_page(current, addr, page); 
		return 0;
	}
	return -1;
}
