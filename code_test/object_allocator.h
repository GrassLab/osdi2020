
typedef struct m_node_t
{
    struct m_node_t *next;
    unsigned long address;
    int size;
} m_node_t;

typedef struct fixed_size_allocator_t
{
    unsigned long space;
    int pages;
    m_node_t free_space;
    m_node_t used_space;
} fixed_size_allocator_t;

fixed_size_allocator_t register_fixed_size_allocator(int pages)
{
    fixed_size_allocator_t allocator;
    allocator.pages = pages;
    allocator.space = get_free_space(pages);

    m_node_t free_space = {
        .next = NULL,
        .address = allocator.space,
        .size = pages * PAGE_SIZE};

    allocator.free_space = free_space;
    allocator.used_space = NULL;

    allocator.used_space = next return allocator;
}

unsigned long fixed_malloc(fixed_size_allocator_t allocator, int size)
{
    struct m_node_t *free_head = allocator.free_space;
    while (free_head != NULL)
    {
        if (free_head->size >= size)
        {
            unsigned long address = free_head->address;
            free_head->address += size;
            free_head->size -= size;

            return address;
        }

        free_head = free_head->next;
    }
}

int register_fixed_size_allocator(int pages)
{
}
