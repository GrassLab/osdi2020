#include "include/pool.h"
#include "include/kernel.h"
#include "include/mm.h"
#include "include/uart.h"

void poolFreeAll(pool *p){
	p->used = p->chunk_per_page- 1;
	p->page = -1;
	p->free =  (free_list *)NULL;
}

void pool_init(pool *p,unsigned long element_size){
	p->element_size = element_size;
	p->chunk_per_page = PAGE_SIZE/element_size;
	poolFreeAll(p);	
	
	for(unsigned int i = 0; i <MAX_POOL_PAGE; i++)
		p->pages[i] =  (struct page_struct *)NULL;
}

unsigned long pool_alloc(pool *p){
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

		if(p->pages[p->page] ==  (struct page_struct *)NULL){
			p->pages[p->page] = alloc_pages(0); //allocate a new page
		}

	}	
	return (unsigned long)(p->pages[p->page]->phy_addr)+ p->used * p->element_size;
}

void pool_free(pool *p,unsigned long ptr){
	free_list *pFree = p->free;

	p->free = (struct free_list*)ptr;
	p->free->next = pFree;		
}

void free_memory_pool(pool *p)
{
	printf("Free page in memory pool\r\n");
	for(int i = 0; i < MAX_POOL_PAGE; ++i) {
		if(p->pages[i] == (struct page_struct *)NULL)
			break;
		else{
			free_page(p->pages[i]->phy_addr);
		}
	}
}

