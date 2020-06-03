#ifndef _MM_ALLOCATOR_H_
#define _MM_ALLOCATOR_H_

void init_buddy_system();
unsigned long get_free_space(unsigned long bytes);
void free_space(unsigned long address);

typedef struct m_node_t
{
    struct m_node_t *next;
    unsigned long address;
    int size;
} m_node_t;

typedef struct varied_size_allocator_t
{
    unsigned long space;
    int pages;
    m_node_t *free_space;
    m_node_t *used_space;
} varied_size_allocator_t;

typedef struct fixed_size_allocator_t
{
    // 1 page = 4 * 256 + 32 * 32 + 256 * 8
    unsigned long address;
    // 4
    int m256[4];
    // 32
    int m32[32];
    // 256
    int m8[256];
} fixed_size_allocator_t;

fixed_size_allocator_t register_fixed_size_allocator();
void *kmalloc_256(fixed_size_allocator_t *allocator);
void *kmalloc_32(fixed_size_allocator_t *allocator);
void *kmalloc_8(fixed_size_allocator_t *allocator);
void free_fixed_memory(fixed_size_allocator_t *allocator, unsigned long address);

varied_size_allocator_t register_varied_size_allocator();
void *kmalloc(varied_size_allocator_t *allocator, unsigned long size);
void free_varied_memory(varied_size_allocator_t *allocator, unsigned long address);

#endif