#include "type.h"
#include "memory/memManager.h"
#include "device/uart.h"

static const uint32_t MAX_PAGES = 5120;
static struct page pages[5120];
static struct buddyList buddy[10];

uint32_t pow(uint32_t num, uint32_t power)
{
    uint32_t result = 1;
    for (uint32_t i = 0; i < power; ++i)
    {
        result *= num;
    }

    return result;
}

struct page* _allocFreePage(uint32_t order)
{
    struct page* free_page = buddy[order].head;
    buddy[order].head = free_page->next;
    if (buddy[order].head != (struct page* )0)
        buddy[order].head->prev = (struct page* )0;
    free_page->next = (struct page* )0;
    buddy[order].size--;

    uartPuts("allocate page: ");
    uartInt(free_page->pfn);
    uartPuts(" with order ");
    uartInt(order);
    uartPuts("\n");

    return free_page;
}

void _dividePage(uint32_t srcOrder, uint32_t dstOrder)
{
    for (uint32_t i = srcOrder; i > dstOrder; --i)
    {
        struct page* free_page = buddy[i].head;
        buddy[i].head = free_page->next;
        if (buddy[i].head != (struct page* )0)
            buddy[i].head->prev = (struct page* )0;
        free_page->next = (struct page* )0;
        free_page->order--;
        buddy[i].size--;

        struct page* new_fp = &pages[free_page->pfn ^ pow(2, i-1)];
        new_fp->order = i - 1;
        new_fp->next = free_page;
        free_page->prev = new_fp;
        buddy[i-1].head = new_fp;
        buddy[i-1].size = 2;

        uartPuts("divide page ");
        uartInt(free_page->pfn);
        uartPuts(" with new page ");
        uartInt(new_fp->pfn);
        uartPuts(" to order ");
        uartInt(new_fp->order);
        uartPuts("\n");
    }
}

void _mergePage(struct page* delete_page)
{
    struct page* buddy_page = &pages[delete_page->pfn ^ pow(2, delete_page->order)];
    if ((buddy_page->order == delete_page->order) && buddy_page->used == false)
    {
        uartPuts("merge page ");
        uartInt(delete_page->pfn);
        uartPuts(" and page ");
        uartInt(buddy_page->pfn);

        buddy[buddy_page->order].size--;

        if (buddy_page->prev == (struct page* )0)
        {
            if (buddy_page->next == (struct page* )0)
                buddy[buddy_page->order].head = (struct page* )0;
            else
            {
                buddy_page->next->prev = (struct page* )0;
                buddy[buddy_page->order].head = buddy_page->next;
            }
        }
        else
        {
            buddy_page->prev = buddy_page->next;
        }

        struct page* new_page;
        if (delete_page->pfn > buddy_page->pfn)
            new_page = buddy_page;
        else
            new_page = delete_page;

        uartPuts(" to page ");
        uartInt(new_page->pfn);
        uartPuts(" of order ");
        uartInt(new_page->order);
        uartPuts("\n");

        new_page->order = delete_page->order + 1;
        buddy[new_page->order].size++;
        if (buddy[new_page->order].head == (struct page* )0)
            buddy[new_page->order].head = new_page;
        else 
        {
            buddy[new_page->order].head->prev = new_page;
            new_page->next = buddy[new_page->order].head;
            buddy[new_page->order].head = new_page;

            _mergePage(new_page);
        }
    }
}

uint64_t getFreePage(uint32_t order)
{
    struct page* free_page = (struct page* )0;
    if (buddy[order].size != 0)
    {
        free_page = _allocFreePage(order);
    }
    else
    {
        for (uint32_t i = order + 1; i < 10; ++i)
        {
            if (buddy[i].size != 0)
            {
                _dividePage(i, order);
                free_page = _allocFreePage(order);
                break;
            }
        }
    }
    
    if (free_page != (struct page* )0)
    {
        free_page->used = true;
        uint64_t page = LOW_MEMORY + free_page->pfn * PAGE_SIZE;
        memzero(page + VA_START, PAGE_SIZE * pow(2, order));
        return page;
    }
            
    return 0;
}

void freePage(uint64_t p)
{
    struct page* delete_page = &pages[(p - LOW_MEMORY) / PAGE_SIZE];
    delete_page->used = false;
    _mergePage(delete_page);
}

void initPage()
{
    for (uint32_t i = 0; i < 5120; ++i)
    {
        pages[i].used = false;
        pages[i].pfn = i;
        pages[i].order = 9;
        pages[i].prev = (struct page* )0;
        pages[i].next = (struct page* )0;
    }

    for (uint32_t i = 0; i < 10; ++i)
    {
        buddy[i].head = (struct page* )0;
        buddy[i].size = 0;
    }

    for (uint32_t i = 0; i < 5120; i += 512)
    {
        buddy[9].size++;

        if (buddy[9].head == (struct page* )0)
            buddy[9].head = &pages[i];
        else 
        {
            buddy[9].head->prev = &pages[i];
            pages[i].next = buddy[9].head;
            buddy[9].head = &pages[i];
        }
    }
}