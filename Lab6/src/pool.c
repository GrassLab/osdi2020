#include "include/pool.h"
#include "include/kernel.h"
#include "include/mm.h"
#include "include/uart.h"

void allocator_init(){
	for(int i=0;i<MAX_ALLOCATOR_NUM;i++)
		allocator_used_map[i] = POOL_NOT_USE;
}

int allocator_register(unsigned long size){
	for(int i=0;i<MAX_ALLOCATOR_NUM;i++){
		if (allocator_used_map[i] == POOL_NOT_USE){
			allocator_used_map[i] = POOL_USE;
			pool_init(&obj_allocator[i],size);
			return i;
		}
	}
	
	printf("register more than 8 allocator, not handle right now");
	return -1;
}

unsigned long allocator_alloc(int allocator_num,int type){
	 if (type == KERNEL_ALLOC)
	 	return pool_alloc_kernel(&obj_allocator[allocator_num]);
	 else if(type == USER_ALLOC)
		return pool_alloc_user(&obj_allocator[allocator_num]);
	 else
		return -1; // error
}

void allocator_free(int allocator_num,unsigned long ptr){
	pool_free(&obj_allocator[allocator_num],ptr);
}

void allocator_unregister(int allocator_num,int type){
	if (type == KERNEL_ALLOC)
		free_kernel_memory_pool(&obj_allocator[allocator_num]);
	else if(type == USER_ALLOC)
		free_user_memory_pool(&obj_allocator[allocator_num]);
	else
		printf("ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");

	allocator_used_map[allocator_num] = POOL_NOT_USE;
}


void poolFreeAll(pool *p){
	p->used = p->chunk_per_page- 1;
	p->page = -1;
	p->free =  (free_list *)NULL;
}

void pool_init(pool *p,unsigned long element_size){
	p->element_size = element_size;
	p->chunk_per_page = PAGE_SIZE/element_size;
	poolFreeAll(p);	
	
	for(unsigned int i = 0; i <MAX_POOL_PAGE; i++){
		p->pages_addr[i].phy_addr = 0;
		p->pages_addr[i].vir_addr = 0;
		p->pages_addr[i].used = POOL_NOT_USE;
	}
}

unsigned long pool_alloc_kernel(pool *p){
	if(p->free != (free_list *)NULL) { //exist chunk in pool
		free_list *recycle = p->free;
		p->free = p->free->next;
		return (unsigned long)recycle;
	}

	p->used++;
	if(p->used >= p->chunk_per_page){ // need a new page and cut to chunk
		p->used = 0;
		p->page++; 
		if(p->page >= MAX_POOL_PAGE) { //no more struct to record the page we use
			printf("Error: no handle\r\n");
			while(1);
		}

		if(p->pages_addr[p->page].used == POOL_NOT_USE){
			p->pages_addr[p->page].used = POOL_USE;
			p->pages_addr[p->page].vir_addr = allocate_kernel_page();  //allocate a new page
			p->pages_addr[p->page].phy_addr = virtual_to_physical(p->pages_addr[p->page].vir_addr);
		}

	}
	
	// for kernel, vir_addr has been inited
	return ((unsigned long)(p->pages_addr[p->page].vir_addr)+ p->used * p->element_size);
}

unsigned long pool_alloc_user(pool *p){
	if(p->free != (free_list *)NULL) { //exist chunk in pool
		free_list *recycle = p->free;
		p->free = p->free->next;
		return (unsigned long)recycle;
	}
	p->used++;
	if(p->used >= p->chunk_per_page){ // need a new page and cut to chunk
		p->used = 0;
		p->page++; 
		if(p->page >= MAX_POOL_PAGE) { //no more struct to record the page we use
			printf("Error: no handle\r\n");
			while(1);
		}

		if(p->pages_addr[p->page].used == POOL_NOT_USE){
			p->pages_addr[p->page].used = POOL_USE;
			int flag;
			unsigned long vir_addr = 0x1000;
			struct mm_struct mm = current->mm;
			
			while(1){		
				flag = 0;	
				for(int i=0;i< mm.vm_area_count;i++){
					if(vir_addr >= mm.mmap[i].vm_start && \
						vir_addr < mm.mmap[i].vm_end){
						
						flag = 1;
						vir_addr = mm.mmap[i].vm_end;
						break;
					}
				}
		
		
				for(int i=0;i<current->mm.user_pages_count;i++){
					if(vir_addr == current->mm.user_pages[i].vir_addr){
						flag = 1;
						vir_addr+=PAGE_SIZE;
						break;
					}
				}

				if(flag == 0)
					break;
			}
			p->pages_addr[p->page].phy_addr = virtual_to_physical(allocate_user_page(current,vir_addr));
			p->pages_addr[p->page].vir_addr = vir_addr;
		}
	}

	return ((unsigned long)(p->pages_addr[p->page].vir_addr)+ p->used * p->element_size);
}

void pool_free(pool *p,unsigned long ptr){
	free_list *pFree = p->free;
	p->free = (struct free_list*)ptr;
	p->free->next = pFree;
}

void free_kernel_memory_pool(pool *p){
	for(int i = 0; i < MAX_POOL_PAGE; ++i) {
		if(p->pages_addr[i].used == POOL_NOT_USE)
			break;
		else{
			free_page(p->pages_addr[i].phy_addr);
		}
	}
}


void free_user_memory_pool(pool *p){
	for(int i = 0; i < MAX_POOL_PAGE; ++i) {
		if(p->pages_addr[i].used == POOL_NOT_USE)
			break;
		else{
			free_user_page(p->pages_addr[i].vir_addr);
		}
	}
}
