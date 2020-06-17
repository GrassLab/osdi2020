#define PBASE 0x3F000000
#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define LOW_MEMORY (2 * SECTION_SIZE)
#define HIGH_MEMORY PBASE

#define PAGING_MEMORY (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES (PAGING_MEMORY / PAGE_SIZE)

#define THREAD_SIZE PAGE_SIZE

static unsigned short mem_map[PAGING_PAGES] = {
    0,
};

unsigned long get_free_page()
{
    for (int i = 0; i < PAGING_PAGES; i++)
    {
        if (mem_map[i] == 0)
        {
            mem_map[i] = 1;
            return LOW_MEMORY + i * PAGE_SIZE;
        }
    }
    return 0;
}

void free_page(unsigned long p)
{
    mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}