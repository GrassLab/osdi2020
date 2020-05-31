#include "uart.h"
#include "str.h"
#include "mailbox.h"
#include "lfb.h"
#include "exc.h"
#include "syscall.h"

#define PAGE_SIZE 4096
#define REAL_PAGE_SIZE (4096)
#define PAGE_POW 9
#define NUM_PAGE (1<<PAGE_POW)
#define LINK_LIST_SIZE (16)
#define ALLOCATED 1
#define NO_ALLOCATED 0
#define USAGE 2

typedef struct link_list{
	char* ptr;
    unsigned long long state;
    struct link_list* next;
} link_list;

/*typedef struct page{
    char used;
} page;*/

__attribute__((section(".userspace"))) link_list all_link[PAGE_POW+1];
__attribute__((section(".userspace"))) char page_frame[NUM_PAGE][PAGE_SIZE];
//__attribute__((section(".userspace"))) page all_page[262144];

void link_init()
{
	for(int i=0;i<PAGE_POW;i++)
    {
        for(int j=0;j<LINK_LIST_SIZE;j++)
        {
            all_link[i].state = NO_ALLOCATED;
            all_link[i].next = 0;
        }
    }
    all_link[PAGE_POW].state = ALLOCATED;
    all_link[PAGE_POW].ptr = page_frame[0];
}

char* mmap(unsigned long long size)
{
    int pow2=1, tmp=0;
    link_list* tmp_next, *final_ret;

    for(int i=0;i<PAGE_POW;i++)
    {
        if((pow2 << i) > size)
        {
            tmp = i;
            break;
        }
    }

    if(all_link[tmp].state == ALLOCATED)
    {
        final_ret
        if(all_link[tmp].next != 0)
        {
            tmp_next = all_link[tmp].next;
            all_link[tmp].ptr = tmp_next->ptr;
            all_link[tmp].state = tmp_next->state;
            all_link[tmp].next = tmp_next->next;
        }
        all_link[tmp].state = USAGE;
        return all_link[tmp].ptr;
    }
    else if(all_link[tmp].next != 0)
    {
        do
        {
            tmp_next = all_link[tmp].next;
            if(tmp_next->state == ALLOCATED)
            {
                tmp_next->state = USAGE;
                return tmp_next->ptr;
            }
        }while(tmp_next->next != 0);
    }





}

void main()
{
    // set up serial console
    uart_init();
	uart_hex(sizeof(link_list));
	while(1);
	
}