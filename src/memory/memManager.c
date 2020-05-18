#include "memory/memManager.h"
#include "memory/mm.h"

static struct page pages[PAGING_PAGES];

uint64_t getFreePage()
{
    for (int i = 0; i < PAGING_PAGES; i++)
    {
        if (pages[i].used == false)
        {
            pages[i].used = true;
            uint64_t page = LOW_MEMORY + i * PAGE_SIZE;
            memzero(page + VA_START, PAGE_SIZE);
            return page;
        }
    }
    return 0;
}

uint64_t allocateKernelPage()
{
    uint64_t page = getFreePage();
    if (page == 0)
    {
        return 0;
    }
    return page + VA_START;
}

void freePage(uint64_t p)
{
    pages[(p - LOW_MEMORY) / PAGE_SIZE].used = false;
}