#include "page.h"
#include "uart.h"
#define PD_TABLE 0b11
#define MAIR_IDX_DEVICE_nGnRnE 0
#define PD_ACCESS (1 << 10)
#define BOOT_PTE_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_TABLE)

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

unsigned long long* page_alloc() //return page virtual address
{
    unsigned long long i = NON_PRESERV_PAGE;
    for(;i<NUM_PAGE;i++) // i is PFN
    {
        if(all_page[i].used == 0)
        {
            all_page[i].used = 1;
            avaliable_pages--;
            return (unsigned long long*)(i * 0x1000);
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

unsigned long long user_paging()
{
    unsigned long long* vir_pgd = page_alloc();
    unsigned long long* vir_pud = page_alloc();
    unsigned long long* vir_pmd = page_alloc();
    unsigned long long* vir_pte = page_alloc();
    *vir_pgd = ((unsigned long long)vir_pud) | 0b11;
    *vir_pud = ((unsigned long long)vir_pmd) | 0b11;
    *vir_pmd = ((unsigned long long)vir_pte) | 0b11;
    *vir_pte = ((unsigned long long)page_alloc()) | BOOT_PTE_ATTR;
    return (unsigned long long)vir_pgd;
}

void memcpy(unsigned long long* from, unsigned long long* to, unsigned long long size)
{
    
}

