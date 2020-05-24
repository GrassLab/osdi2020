#include "page.h"
#include "uart.h"


__attribute__((section(".userspace"))) page all_page[262144];

int avaliable_pages = 262144;

unsigned long long virtual_to_physical(unsigned long long vir){
	unsigned long long pfn = (vir&0x0000FFFFFFFFFFFF)>>12;
	unsigned long long offset = (vir<<52)>>52;

	return pfn*PAGE_SIZE | offset;
}

void page_struct_init()
{
    //extern void* _pgt_start; 
    //uart_hex((unsigned long long)&_pgt_start);
    unsigned long long i=0;
    for(;i<NON_PRESERV_PAGE;i++)
    {
        all_page[i].used = 1;
        avaliable_pages--;
    }
    for(;i<NUM_PAGE;i++)
    {
        all_page[i].used = 0;
    }
}

unsigned long long page_alloc() //return kernel virtual address
{
    unsigned long long i = NON_PRESERV_PAGE;
    for(;i<NUM_PAGE;i++) // i is PFN
    {
        if(all_page[i].used == 0)
        {
            all_page[i].used = 1;
            avaliable_pages--;
            return ((i * 0x1000) | 0xffff000000000000);
        }
    }
    return 0;
}

void page_free(unsigned long long vir)
{
    unsigned long long pfn = (vir&0x0000FFFFFFFFFFFF)>>12;
    all_page[pfn].used = 0;
    avaliable_pages++;
}


void memcpy(unsigned long long* from, unsigned long long* to, unsigned long long size)
{
    
    for(unsigned long long i = 0; i < size; i++)
    {
       ((char*)to)[i] = ((char*)from)[i];
    }
}

