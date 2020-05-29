#include "mm_allocator.h"
#include "printf.h"
#include "uart.h"

#define PAGE_SIZE 4096

typedef struct mem_page_t
{
    unsigned char c[PAGE_SIZE];
} mem_page_t;

mem_page_t mem_pages[1024];

typedef struct page_t
{
    unsigned long pfn;
    int used;
} page_t;

page_t page_table[1024];

typedef struct buddy_t
{
    int used;
    int u;
    int size;
    unsigned long pa;
} buddy_t;

buddy_t buddy_pool[1024];

// 0: 4KB
// 1: 8KB
// 10: (2 ** 10) * 4KB = 4096KB

void init_buddy_system()
{
    buddy_t b = {
        .used = 0,
        .u = 10,
        .size = 1024, // pages
    };

    for (int i = 0; i < 1024; i++)
    {
        buddy_t b = {
            .used = 0,
            .u = 0,
            .size = 0,
            .pa = (unsigned long)&mem_pages[i]};
        buddy_pool[i] = b;
    }
    buddy_pool[0] = b;
}

// return address
unsigned long get_free_space(unsigned long bytes)
{
    int i = 0;

    // bytes to 4KB
    int u = (bytes >> 12);
    if (bytes % PAGE_SIZE != 0)
        u++;

    printf("u = %d\n", u);
    while (i < 1024)
    {
        buddy_t *b = &buddy_pool[i];
        if (b->used)
        {
            i += b->size;
        }
        else
        {
            if ((b->size / 2) < u && (b->size >= u))
            {
                b->used = 1;
                printf("[%d, size:%d] request %d, get %d!\n", i, b->size, bytes, b->size << 12);
                return (unsigned long)&mem_pages[i];
            }
            else if (b->size > u)
            {
                b->size /= 2;
                b->u -= 1;

                buddy_t *buddy = &buddy_pool[i + b->size];
                buddy->used = 0;
                buddy->u = b->u;
                buddy->size = b->size;

                printf("[%d, size:%d] split %d -> %d, ", i, b->size * 2, b->size * 2, b->size);
                printf("block [%d, size:%d] [%d, size:%d]\n", i, buddy_pool[i].size, i + b->size, buddy_pool[i + b->size].size);

                char *s;
                //uart_puts(s);
            }
            else
            {
                printf("[%d, size:%d] find next block\n", i, b->size);
                i += b->size;
            }
        }
    }

    printf("Can't find match block!\n");

    return 0;
}

void merge_buddy(int pfn)
{
    buddy_t *b = &buddy_pool[pfn];
    if (b->used)
        return;

    int buddy_idx = pfn ^ (1 << b->u);
    buddy_t *buddy = &buddy_pool[buddy_idx];
    printf("buddy index %d\n", buddy_idx);
    if (buddy->used)
    {
        return;
    }

    // merge
    // b.u and buddy.u most be same
    if (buddy->u != b->u)
    {
        printf("Error: merge buddy, different order\n");
        while (1)
        {
        }
    }

    printf("Merge buddy [%d] [%d]\n", pfn, buddy_idx);
    if (pfn > buddy_idx)
    {
        pfn = buddy_idx;
        buddy_t tmp = *b;
        *b = *buddy;
        *buddy = tmp;
    }
    b->u = b->u + 1;
    b->size = b->size * 2;
    buddy->u = 0;
    buddy->size = 0;
    printf("After merge [%d, size: %d, used: %d]\n", pfn, b->size, b->used);
}

void free_space(unsigned long address)
{
    int pfn = (address - (unsigned long)mem_pages) / PAGE_SIZE;
    buddy_t *b = &buddy_pool[pfn];
    b->used = 0;

    merge_buddy(pfn);
}