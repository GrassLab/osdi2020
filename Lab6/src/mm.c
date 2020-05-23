#include "include/mm.h"
#include "include/uart.h"
#include "include/arm/sysreg.h"
#include "include/scheduler.h"
#include "include/kernel.h"

int remain_page = PAGE_ENTRY;

//#####
struct page_struct* get_pages_from_list(int order){
	int neworder = order;
	struct page_struct *page;

	int flag = 0;
	for(;neworder<MAX_ORDER;neworder++){
		//no buddy in target order, so find a larger one
		if(list_empty(&page_buddy[neworder])) 
			continue;
		else{
			page = list_entry(page_buddy[neworder].next,struct page_struct,list);	
			
			struct list_head *next_buddy = (&(BUDDY_END(page,neworder)->list))->next;
			
			// take the needed page and link else back
			next_buddy->prev = &page_buddy[neworder];
			page_buddy[neworder].next = next_buddy->next;
			flag = 1;
			break;
		}

	}	

	printf("select page: %d\r\n",page->page_num);
	struct list_head *tlst,*tlst1;
	//printf("### required order %d, get from order %d\r\n",order,neworder);
	if(flag==0)
		return (struct page_struct*)NULL;
	
	// if we take buddy larger then we required, give back those we don't nedd
	for(neworder--;neworder>=order;neworder--){
		tlst1 =&(BUDDY_END(page,neworder)->list);
		tlst = &(page->list);
		
		// take bottom half 
		page = NEXT_BUDDY_START(page,neworder);
		// revise the top half order of this page
		list_entry(tlst,struct page_struct,list)->order=neworder;
                // and add back to buddy list
		list_add_chain_tail(tlst,tlst1,&page_buddy[neworder]);
	}

	page->used = USED_NOW;
	page->order = order;
	
	return page;
}

struct page_struct *alloc_pages(int order){
	struct page_struct *page = get_pages_from_list(order);
	if(page==(struct page_struct*)NULL)
		return (struct page_struct*)NULL;
	
	// set all these continuous page to used
	for(int i=0;i<(1<<order);i++){
		(page+i)->used = USED_NOW;
		printf("Using page %d \r\n", (page+i)->page_num);
		remain_page--;
		memzero(page[i].phy_addr + VA_START, PAGE_SIZE);
	}
	return page;
}

unsigned long get_free_page(int order){
	struct page_struct* page;
	page = alloc_pages(order);
	if (!page)
		return -1;
	return	page->phy_addr;
}

unsigned long allocate_kernel_page(int order){
	unsigned long page = get_free_page(order);
	if(page == 0){
		return 0;
	}
	return page + VA_START;
}

unsigned long allocate_user_page(int order,struct task_struct *task, \
		unsigned long vir_addr){

	unsigned long page = get_free_page(order); 
	if(page == 0){
		return 0;
	}
	map_page(task,vir_addr,page,MMU_PTE_FLAGS); // maps it to the provided virtual address
	return page + VA_START;
}

void map_page(struct task_struct *task, unsigned long vir_addr, \
		unsigned long page,unsigned long page_attr)
{
	unsigned long pgd;
	
	// If it is the first time to map this task
	if(!task->mm.pgd){
		task->mm.pgd = get_free_page(0);
		task->mm.kernel_pages[task->mm.kernel_pages_count++] = task->mm.pgd;
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
	map_entry((unsigned long *)(table+VA_START), vir_addr, page, page_attr);

	struct user_page p = {page, (vir_addr>>12)<<12};
	task->mm.user_pages[task->mm.user_pages_count++] = p;
}

unsigned long map_table(unsigned long *table, unsigned long shift, \
	       	unsigned long vir_addr, struct task_struct *task) {
   	
	unsigned long index = vir_addr >> shift;
    	index = index & (PTRS_PER_TABLE - 1);
    	if(!table[index]){	
        	unsigned long next_level_table = get_free_page(0);
        	unsigned long entry = next_level_table | PD_TABLE;
        	table[index] = entry;
        	task->mm.kernel_pages[task->mm.kernel_pages_count++] = next_level_table;
	
		return next_level_table;
	}
	else{  // case that child table was already allocated 
		return (table[index]>>12)<<12;
	}
}

void map_entry(unsigned long *pte, unsigned long vir_addr,\
	       	unsigned long phy_addr,unsigned long page_attr) {

    unsigned long index = vir_addr >> 12;
    index = index & (PTRS_PER_TABLE - 1);
    unsigned long entry = phy_addr | page_attr;
    pte[index] = entry;
}

void free_page(unsigned long p){ //input should be physical address
	//BUGGGGGGGGGGGGGGG!
	unsigned long pfn = physical_to_pfn(p);
	printf("Free Page %d\r\n", pfn);	
	if(page[pfn].used==USED_NOW)
		page[pfn].used = NOT_USED;
	remain_page++;
}

void memcpy (void *dest, const void *src, unsigned long len)
{
  	char *d = dest;
  	const char *s = src;
  	while (len--)
    		*d++ = *s++;
}

void buddy_init(void){
	
	for(int i=0;i<MAX_ORDER;i++){
		INIT_LIST_HEAD(&page_buddy[i]);
	}
}


void init_page_struct(){
	
	buddy_init();
	// reset page struct
	int i = 0;	
	for(;i<FIRST_AVAILIBLE_PAGE;i++){
		page[i].used = PRESERVE; // for the preserve page, they should not been alloc
		remain_page--;
	}
	
	int usable = i;	
	int counter = 0;
	for(;i<PAGE_ENTRY;i++){
		page[i].used = NOT_USED;
		page[i].phy_addr = i * PAGE_SIZE;
		page[i].page_num = i;

		INIT_LIST_HEAD(&(page[i].list));
		
		// making the memory max buddy as possible*/
		if(counter == 0){
			if( i + PAGE_NUM_FOR_MAX_BUDDY < PAGE_ENTRY+1 ){
				page[i].order = MAX_ORDER-1;
				list_add_tail(&(page[i].list),&page_buddy[MAX_ORDER-1]);
				
				counter = PAGE_NUM_FOR_MAX_BUDDY-1;
			} 
		// the remainder not enough to merge into a max buddy is done as min buddy
			else{
				page[i].order=0;
				list_add_tail(&(page[i].list),&page_buddy[0]);
			}
		}
		else{
			page[i].order=-1;
			list_add_tail(&(page[i].list),&page_buddy[MAX_ORDER-1]);
			counter--;
		}
	}
	
	//dump
	for(i=usable;i<usable+513;i++){
		if(page[i].order!=-1){
			printf("page number: %d /",i);
			printf("page order %d\r\n",page[i].order);
		}
		if(page[i].order==0){
			printf("page number: %d /",i);
			printf("page order %d\r\n",page[i].order);
			break;
		
		}
	}
	printf("last entry %d\r\n",PAGE_ENTRY);
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

                 printf("%2x ",*s);
		 if(count>=16){
			 count=0;
			 printf("\r\n");
		 }
                 s++;
         }
	 printf("\r\n");
}

int copy_virt_memory(struct task_struct *dst){
	// copy virtual memory

	for(int i=0;i<current->mm.user_pages_count;i++){
		struct user_page src = current->mm.user_pages[i];
		unsigned long page = allocate_user_page(0,dst, src.vir_addr);
		if(!page)
			return -1;

		memcpy((void *)page,(void *)((src.vir_addr>>12)<<12),PAGE_SIZE); //page aligned
	}

	// copy vm area struct
	dst->mm.vm_area_count = current->mm.vm_area_count;
	for(int i=0;i<dst->mm.vm_area_count;i++)
		dst->mm.mmap[i] = current->mm.mmap[i];
	return 0;
}

int page_fault_handler(unsigned long addr,unsigned long esr){
	printf("+++ Page fault at 0x%x\r\n",addr);
	if(((esr>>2)&0x3) != 1){ //If not a translation fault, kill  
  		 switch((esr>>2)&0x3) {
 			  case 0: uart_send_string("Address size fault, "); break;
                          case 2: uart_send_string("Access flag fault, "); break;
                          case 3: uart_send_string("Permission fault, "); break;
                  }
 
                 printf("data abort at 0x%x, killed\r\n",addr);
                 exit_process();
                 return -1;
         }

	// Else check if user access a map region
	struct mm_struct mm = current->mm;
	for(int i=0;i< mm.vm_area_count;i++){
		if( (addr >= mm.mmap[i].vm_start) && (addr < mm.mmap[i].vm_end)){	
			unsigned long page = get_free_page(0);
			if (page == 0) 
            			return -1;
        		
			unsigned long long page_attr;	
			int prot = mm.mmap[i].vm_prot;
			if(prot == 0){ //non accessible
				page_attr = MMU_NONE;
			}
			else{
				page_attr = MMU_PTE_FLAGS;
				if( (prot&0b110) == 0b100){ //read only
					page_attr |= PD_READONLY;
				}
				if( (prot&0b001) == 0b000){ //non exec
					page_attr |= PD_NON_EXEC_EL0;
				}
			}
			//For file, copy the file content to the memory region.
			struct vm_area_struct vm_area = current->mm.mmap[i];
			if( (char *)vm_area.file_start != NULL){
				memcpy((void *)page, \
					(void*)(vm_area.file_start +  vm_area.file_offset),\
					    vm_area.vm_end - vm_area.vm_start);
			}	
			map_page(current, addr, page, page_attr);		
			return 0;
		}
	}

	// If not a map region, kill
	printf("### Page fault address at 0x%x, killed\r\n",addr);
	exit_process(); 	
	return -1;
		
}

 void* mmap(void* addr, unsigned long len, int prot, int flags, void* file_start, int file_offset){
	unsigned long vir_addr;

	if(file_start!=NULL && file_offset!=0){
		printf("Map to file: not implement yet");
		while(1);
	}
			
	// For address:
	// addr should be page aligned 	
	if(addr!=NULL)
		vir_addr = ((unsigned long)(addr)>>12)<<12;
	else{
		vir_addr = 0x1000;
	}

	struct mm_struct mm = current->mm;
	int flag;
	
	//not so smart...... but anyway	
	while(1){		
		// kernel decides the new region’s start address if addr is invalid				 // First, make sure new region not overlap exist region
		flag = 0;	
		for(int i=0;i< mm.vm_area_count;i++){
			if(vir_addr == mm.mmap[i].vm_start){
				flag = 1;
				vir_addr+=PAGE_SIZE;
				break;
			}
		}
		
		// Next, make sure new region not overlap exist page
		for(int i=0;i<current->mm.user_pages_count;i++){
			if(vir_addr == current->mm.user_pages[i].vir_addr){
				flag = 1;
				vir_addr+=PAGE_SIZE;
				break;
			}
		}

		if(flag==0)
			break;
	}
		
		
	if(addr!=NULL && vir_addr != (unsigned long)addr){
		printf("!!! You can't use address 0x%x\r\n", addr);		
		
		if(flags==MAP_FIXED){
			printf("!!! mmap failed\r\n");
			return NULL;
		}
		else
			printf("!!! Map to vir addr at 0x%x\r\n",vir_addr);
	}	
		
	// For len:
	// Memory region created by mmap should be page aligned
	if( len % PAGE_SIZE != 0)
		len += PAGE_SIZE - (len % PAGE_SIZE);
	
	struct vm_area_struct *vm_area = &current->mm.mmap[current->mm.vm_area_count];
	vm_area->vm_start = vir_addr;
	vm_area->vm_end = vir_addr + len;
	vm_area->vm_prot = prot;
	vm_area->file_start = 0; 
	vm_area->file_offset = 0;
	
	current->mm.vm_area_count++;
		
	return (void *)vir_addr;
 }

