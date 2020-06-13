#include "mm_allocator.h"
#include "printf.h"
#include "uart.h"
#include "common.h"

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

typedef struct m_node_pool_t
{
    m_node_t node;
    int used;
} m_node_pool_t;
m_node_pool_t m_node_pool[1024];

m_node_t *get_m_node()
{
    for (int i = 0; i < 1024; i++)
    {
        if (m_node_pool[i].used == 0)
        {
            m_node_pool[i].used = 1;
            return &m_node_pool[i].node;
        }
    }
    return NULL;
}

void free_m_node(m_node_t *node)
{
    for (int i = 0; i < 1024; i++)
    {
        if (node == &m_node_pool[i].node)
        {
            if (m_node_pool[i].used == 1)
                m_node_pool[i].used = 0;
            else
                printf("Error: double free m node\n");
            return;
        }
    }
    printf("Error: wrong address\n");
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
    merge_buddy(pfn);
}

void free_space(unsigned long address)
{
    int pfn = (address - (unsigned long)mem_pages) / PAGE_SIZE;
    buddy_t *b = &buddy_pool[pfn];
    b->used = 0;

    merge_buddy(pfn);
}

/* object allocator */
varied_size_allocator_t register_varied_size_allocator()
{
    unsigned long pages = 1;
    varied_size_allocator_t allocator;
    allocator.pages = pages;
    allocator.space = get_free_space(pages * PAGE_SIZE);

    m_node_t *free_space = get_m_node();
    free_space->address = allocator.space;
    free_space->next = NULL;
    free_space->size = pages * PAGE_SIZE;

    allocator.free_space = free_space;
    allocator.used_space = NULL;

    return allocator;
}

void *kmalloc(varied_size_allocator_t *allocator, unsigned long size)
{
    m_node_t *free_head = allocator->free_space;
    while (free_head != NULL)
    {
        if (free_head->size >= size)
        {
            unsigned long address = free_head->address;
            free_head->address += size;
            free_head->size -= size;

            m_node_t *node = get_m_node();
            node->address = address;
            node->size = size;
            node->next = allocator->used_space;
            allocator->used_space = node;

            printf("Allocate varied object: address ");
            uart_send_hex(address >> 32);
            uart_send_hex(address);
            printf(" size %d\n", size);
            return (void *)address;
        }
        free_head = free_head->next;
    }
    printf("Error: kmalloc fail, not enough memory\n");
    return NULL;
}

void free_varied_memory(varied_size_allocator_t *allocator, unsigned long address)
{
    m_node_t *used_head = allocator->used_space;
    m_node_t *prev = NULL;
    while (used_head != NULL)
    {
        if (used_head->address == address)
        {
            if (prev == NULL)
            {
                allocator->used_space = used_head->next;
            }
            else
            {
                prev->next = used_head->next;
            }
            used_head->next = allocator->free_space;
            allocator->free_space = used_head;

            printf("Free varied object: address ");
            uart_send_hex(address >> 32);
            uart_send_hex(address);
            printf(" size %d\n", used_head->size);
            return;
        }
        prev = used_head;
        used_head = used_head->next;
    }
    printf("Error: double free, or you don't allocate thie memory\n");
}

fixed_size_allocator_t register_fixed_size_allocator()
{
    fixed_size_allocator_t allocator;
    allocator.address = get_free_space(1);
    for (int i = 0; i < 4; i++)
        allocator.m256[i] = 0;
    for (int i = 0; i < 32; i++)
        allocator.m32[i] = 0;
    for (int i = 0; i < 256; i++)
        allocator.m8[i] = 0;

    return allocator;
}

void *kmalloc_256(fixed_size_allocator_t *allocator)
{
    for (int i = 0; i < 4; i++)
    {
        if (allocator->m256[i] == 0)
        {
            allocator->m256[i] = 1;
            unsigned long address = allocator->address + i * 256;
            printf("Allocate fixed object: address ");
            uart_send_hex(address >> 32);
            uart_send_hex(address);
            printf(" size 256\n");
            return (void *)address;
        }
    }
    return NULL;
}

void *kmalloc_32(fixed_size_allocator_t *allocator)
{
    for (int i = 0; i < 32; i++)
    {
        if (allocator->m32[i] == 0)
        {
            allocator->m32[i] = 1;
            unsigned long address = allocator->address + 4 * 256 + i * 32;
            printf("Allocate fixed object: address ");
            uart_send_hex(address >> 32);
            uart_send_hex(address);
            printf(" size 32\n");
            return (void *)address;
        }
    }
    return NULL;
}

void *kmalloc_8(fixed_size_allocator_t *allocator)
{
    for (int i = 0; i < 256; i++)
    {
        if (allocator->m8[i] == 0)
        {
            allocator->m8[i] = 1;
            unsigned long address = allocator->address + 4 * 256 + 32 * 32 + i * 8;
            printf("Allocate fixed object: address ");
            uart_send_hex(address >> 32);
            uart_send_hex(address);
            printf(" size 8\n");
            return (void *)address;
        }
    }
    return NULL;
}

void free_fixed_memory(fixed_size_allocator_t *allocator, unsigned long address)
{
    unsigned long offset = address - allocator->address;
    int index;
    int size;
    if (address < allocator->address)
    {
        printf("Error: address out of allocator memory");
        return;
    }
    if (offset < 4 * 256)
    {
        index = offset / 256;
        allocator->m256[index] = 0;
        size = 256;
    }
    else if (offset < 4 * 256 + 32 * 32)
    {
        index = (offset - 4 * 256) / 32;
        allocator->m32[index] = 0;
        size = 32;
    }
    else if (offset < 4 * 256 + 32 * 32 + 256 * 8)
    {
        index = (offset - 4 * 256 - 32 * 32) / 8;
        allocator->m8[index] = 0;
        size = 8;
    }
    else
    {
        printf("Error: address out of allocator memory");
    }
    printf("Free fixed object: address ");
    uart_send_hex(address >> 32);
    uart_send_hex(address);
    printf(" size %d\n", size);
}