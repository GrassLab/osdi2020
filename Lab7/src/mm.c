#include "include/mm.h"
#include "include/uart.h"
#include "include/arm/sysreg.h"
#include "include/scheduler.h"
#include "include/kernel.h"
#include "include/utils.h"
#include "include/pool.h"

int remain_page = PAGE_ENTRY;

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
			page_buddy[neworder].next = next_buddy;
			flag = 1;
			break;
		}

	}	

	//printf("$$$ select page: %d: required order %d and origin order %d\r\n",page->page_num,order,neworder);
	
	if(flag==0) //OOM
		return (struct page_struct*)NULL;
	
	// if we take buddy larger then we required, give back those we don't nedd
	for(neworder--;neworder>=order;neworder--){
			
		struct page_struct* start_page = NEXT_BUDDY_START(page,neworder); 	
		struct page_struct* end_page = BUDDY_END(start_page,neworder);
		
		struct list_head* next_buddy_start =  &(start_page->list);    
		struct list_head* next_buddy_end = &(end_page->list);
		
		/*printf("@@@ cutoff from %d to %d(totally %d page) in order %d list\r\n",\
				start_page->page_num,end_page->page_num, \
				end_page->page_num - start_page->page_num+1, neworder);*/
		// revise the bottom half order of this page
		list_entry(next_buddy_start,struct page_struct,list)->order=neworder;
                // add back the chain to buddy list
		list_add_chain_tail(next_buddy_start,next_buddy_end,&page_buddy[neworder]);
	}

	page->used = USED_NOW;
	page->order = order;
	
	//printf("$$$ alloc from %d to %d\r\n",page->page_num,(BUDDY_END(page,order)->page_num));
	return page;
}

struct page_struct *alloc_pages(int order){
	struct page_struct *page = get_pages_from_list(order);
	if(page==(struct page_struct*)NULL)
		return (struct page_struct*)NULL;
	
	// set all these continuous page to used
	for(int i=0;i<(1<<order);i++){
		(page+i)->used = USED_NOW;
		//printf("Using page %d \r\n", (page+i)->page_num);
		remain_page--;
		memzero(page[i].phy_addr + VA_START, PAGE_SIZE);
	}
	return page;
}

unsigned long kmalloc(unsigned long size){
	// If the allocated size is larger than the maximum size of allocator,
	// it calls page allocator directly to allocate contiguous page frames.
	if(size > (MIN_DEFAULT_ALLOCATOR_SIZE * DEFAULT_ALLOCATOR_NUM)){ 
		int order;
		for(int i=0;i<MAX_ORDER;i++){
			if(size <= (unsigned long)(1<<i)*PAGE_SIZE){
				order = i;
				break;
			}
		}

		unsigned long page = get_free_page(order);
		if(page == 0){
			return 0;
		}
		
		//printf("*** allocate from buudy system order %d\r\n",order);
		return page + VA_START;
	}
	else{
		int allocator_num;
		for(int i=0;i<DEFAULT_ALLOCATOR_NUM;i++){
			if(size <= (unsigned long)MIN_DEFAULT_ALLOCATOR_SIZE*(i+1)){
				allocator_num = i;
				break;
			}
		}
		//printf("*** allocate from allocator number %d\r\n",allocator_num);
		return pool_alloc_kernel(&(default_allocator[current->pid][allocator_num]));	
	}
}

unsigned long get_free_page(int order){
	struct page_struct* page;
	page = alloc_pages(order);
	if (!page)
		return -1;
	return	page->phy_addr;
}

unsigned long allocate_kernel_page(){
	unsigned long page = get_free_page(0);
	if(page == 0){
		return 0;
	}
	return page + VA_START;
}

unsigned long allocate_user_page(struct task_struct *task, \
		unsigned long vir_addr){

	unsigned long page = get_free_page(0); 
	if(page == 0){
		return 0;
	}
	map_page(task,vir_addr,page,MMU_PTE_FLAGS); // maps it to the provided virtual address
	return page + VA_START;
}

// this function can just mapping one page 
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


void put_pages_to_list(struct page_struct *page,int order){
	struct page_struct *tprev,*tnext;

	//printf("+++ put back page %d\r\n",page->page_num);	
	for(; order<MAX_ORDER ;order++){
		tnext=NEXT_BUDDY_START(page,order);
		tprev=PREV_BUDDY_START(page,order);
		
		//printf("prev order %d, num %d\r\n",tprev->order,tprev->page_num);	
		//printf("next order %d, num %d\r\n",tnext->order,tnext->page_num);	
		// if find neighbor with same order exist, then merge!
		if((tnext->used == NOT_USED) && (tnext->order==order)){
			page->order++;
			tnext->order = -1;
			// remove the chain in original list
			list_remove_chain(&(tnext->list),&(BUDDY_END(tnext,order)->list));
			//printf(">>> merge %d to %d\r\n",tnext->page_num,BUDDY_END(tnext,order)->page_num);
			
			// merge them together
			BUDDY_END(page,order)->list.next=&(tnext->list);
			tnext->list.prev=&(BUDDY_END(page,order)->list);
			
			if(order+1 == MAX_ORDER)
				break;
			else
				continue;
		}
		else if((tprev->used == NOT_USED) && (tprev->order==order)){
			page->order=-1;
			// remove the chain in original list
			list_remove_chain(&(tprev->list),&(BUDDY_END(tprev,order)->list)); //**
			
			//printf("<<< merge %d to %d\r\n",tprev->page_num,BUDDY_END(tprev,order)->page_num);
			//merge them together
			BUDDY_END(tprev,order)->list.next=&(page->list);
			page->list.prev=&(BUDDY_END(tprev,order)->list);

			page=tprev;
			page->order++;
			if(order+1 == MAX_ORDER)
				break;
			else	
				continue;
		}
		else{
			break;
		}
	}

	//printf("+++ merge from page %d to %d in order %d\r\n",page->page_num,BUDDY_END(page,order)->page_num,order);
	list_add_chain(&(page->list),&(BUDDY_END(page,order)->list),&page_buddy[order]);
}

// free memory allocate by kmalloc
void kfree(unsigned long p){
	// if your memory was allocated from pool, put it back
	for(int i=0;i<DEFAULT_ALLOCATOR_NUM;i++){
 		pool tmp_pool = default_allocator[current->pid][i];
		for(int page=0; page<=tmp_pool.page; page++){
			if ( ((p>>12)<<12) == (((tmp_pool.pages_addr[page].vir_addr)>>12)<<12)){
				pool_free(&default_allocator[current->pid][i],p);
				//printf("*** free to allocator number %d\r\n",i);
				return;	
			}
		}
 	}

	//printf("*** free to buddy system\r\n");
	// else free it to buddy system
	free_page(p);
}
int free_page(unsigned long p){ //input should be physical address
	unsigned long pfn = physical_to_pfn(p);
	
	int order = page[pfn].order;
	for(unsigned int i=pfn; i < ( pfn+(1<<order)); i++){
		page[i].used = NOT_USED;
		remain_page++;
		//printf("Free Page %d\r\n", i);
	}
	
	put_pages_to_list(&page[pfn],order);
	
	return order;
}

/*************************************************************************/
unsigned long get_table(unsigned long *table, unsigned long shift, \
                 unsigned long vir_addr) {
 
         unsigned long index = vir_addr >> shift;
         index = index & (PTRS_PER_TABLE - 1);
   
         return (table[index]>>12)<<12;
}

void clean_entry(unsigned long *pte, unsigned long vir_addr) {
        unsigned long index = vir_addr >> 12;
        index = index & (PTRS_PER_TABLE - 1);
        pte[index] = 0; //making page fault happened
}

// For user page, things become more complicated, you have to clean up 
// 1. relative vm_struct
// 2. page entry of virtual address
// 3. physical page which the virtual page mapping
int free_user_page(unsigned long vir_addr){	
	
	vir_addr = (vir_addr>>12)<<12;

	// Step 1. If the vir_addr map to a vm_struct, clean it
	// this is for reuse of virtual address
	int count = current->mm.vm_area_count;
	unsigned long vm_end = vir_addr + PAGE_SIZE;
	for(int i=0;i<count;i++){	
		if(vir_addr >= current->mm.mmap[i].vm_start &&\
			vir_addr < current->mm.mmap[i].vm_end){
			
			vm_end = current->mm.mmap[i].vm_end;
			// moving array
			for(int n=i;n<count-1;n++){
				current->mm.mmap[n] = current->mm.mmap[n+1];
			}	
			current->mm.vm_area_count--;		
			break;
		}
	}

	// Step 2+3. free "all" user pages and its page table
	// For user, all using page will be order 0
	// but note that the vir_addr may use more than one page
	count = current->mm.user_pages_count;
	for(int i=0;i<count;i++){
		if(current->mm.user_pages[i].vir_addr >= vir_addr &&\
			current->mm.user_pages[i].vir_addr < vm_end){
			//printf("+++ free page vir addr 0x%x, phy addr 0x%x\r\n",current->mm.user_pages[i].vir_addr,current->mm.user_pages[i].phy_addr);
			free_page(current->mm.user_pages[i].phy_addr);
			// moving array
			for(unsigned int n=i;n<current->mm.user_pages_count-1;n++){
				current->mm.user_pages[n] = current->mm.user_pages[n+1];
			}	
			current->mm.user_pages_count--;
			
			// find table entry and clean it
			unsigned long table = current->mm.pgd;
			unsigned long target_va = current->mm.user_pages[i].vir_addr;
			int shift = 39;
			
			for(int iter=0;iter<3;iter++){
				table = get_table((unsigned long *)(table+VA_START),shift,vir_addr); 
				shift-=9;
			}
			clean_entry((unsigned long *)(table+VA_START),target_va);
		}
	}
	
	
	set_pgd(current->mm.pgd); // after clean, setup pgd 
	return 0; 
}
/**************************************************************************************/

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
		
	int counter = 0;
	for(int i=0;i<PAGE_ENTRY;i++){
		page[i].used = NOT_USED;
		page[i].phy_addr = LOW_MEMORY + i * PAGE_SIZE;
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
	
}



unsigned long virtual_to_physical(unsigned long vir){
	unsigned long pfn = (vir<<16)>>16;
	unsigned long offset = (vir<<52)>>52;
	pfn = (pfn)>>PTE_SHIFT;

	return pfn*PAGE_SIZE | offset;
}

unsigned long physical_to_pfn(unsigned long phy){
	return (phy-LOW_MEMORY)>>12;	
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

	for(unsigned int i=0;i<current->mm.user_pages_count;i++){
		struct user_page src = current->mm.user_pages[i];
		unsigned long page = allocate_user_page(dst, src.vir_addr);
		if(!page)
			return -1;

		memcpy((void *)page,(void *)((src.vir_addr>>12)<<12),PAGE_SIZE); //page aligned
	}

	// copy vm area struct
	dst->mm.vm_area_count = current->mm.vm_area_count;
	for(unsigned int i=0;i<dst->mm.vm_area_count;i++)
		dst->mm.mmap[i] = current->mm.mmap[i];
	return 0;
}

int page_fault_handler(unsigned long addr,unsigned long esr){
	//printf("+++ Page fault at 0x%x\r\n",addr);
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
	for(unsigned int i=0;i< mm.vm_area_count;i++){
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
		for(unsigned int i=0;i< mm.vm_area_count;i++){
			if(vir_addr >= mm.mmap[i].vm_start && vir_addr < mm.mmap[i].vm_end){
				flag = 1;
				vir_addr = mm.mmap[i].vm_end;
				break;
			}
		}
		
		// Next, make sure new region not overlap exist page
		for(unsigned int i=0;i<current->mm.user_pages_count;i++){
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

