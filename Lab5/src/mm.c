#include "include/mm.h"
#include "include/printf.h"
#include "include/uart.h"
#include "include/user.h"

struct page_struct page[PAGE_ENTRY];

unsigned long get_free_page()
{
	// Start from first availible memory
	// Since some region are used for kernel image / stack
	
	for (int i = FIRST_AVAILIBLE_PAGE; i < PAGE_ENTRY; i++){
		// finding availible memory space for your process
		if (page[i].used == NOT_USED){
			printf("Using Page: %d\r\n",i);
			page[i].used = USED_NOW;

			// initialize to zero
			unsigned long page = i*PAGE_SIZE;
			memzero(VA_START+page,PAGE_SIZE);
			return page;
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

void free_page(unsigned long p){
	printf("Free Page %d\r\n",p/PAGE_SIZE);
	if(page[p/PAGE_SIZE].used==USED_NOW)
		page[p / PAGE_SIZE].used = NOT_USED;
	
	// else...error?!
}

void fork_memcpy (void *dest, const void *src, unsigned long len)
{
  	char *d = dest;
  	const char *s = src;
  	while (len--)
    		*d++ = *s++;
}

void init_page_struct(){
	memzero((unsigned long)page, sizeof(struct page_struct)*PAGE_ENTRY);
	
	unsigned long kernel_end = (unsigned long)&_kernel_end;	
	unsigned long user_end = (unsigned long)&_user_end;

	printf("kernel end at: 0x%x%x\r\n",kernel_end>>32,kernel_end);
	printf("user end at: 0x%x%x\r\n",user_end>>32,user_end);
	printf("Stack at 0x%x%x\r\n",(VA_START+LOW_MEMORY)>>32,(VA_START+LOW_MEMORY));
	
	unsigned long pfn;	
	pfn = virtual_to_pfn(LOW_MEMORY);
	
	for(int i=0;i<pfn;i++){
		//these memory should not be allocted
		page[i].used = USED_PRESERVE; 	
	}
}

unsigned long virtual_to_pfn(unsigned long vir){
	// using [47:21] being fpn for 2M section
	unsigned long pfn = (vir<<16)>>16;
	pfn = (pfn)>>PTE_SHIFT;

	return pfn;	
}

unsigned long virtual_to_physical(unsigned long vir){
	unsigned long pfn = (vir<<16)>>16;
	unsigned long offset = (vir<<52)>>52;
	pfn = (pfn)>>PTE_SHIFT;

	return pfn*PAGE_SIZE | offset;
}
