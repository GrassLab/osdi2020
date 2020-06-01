#define page_size (1 << 12)
#define VA_START 0xffff000000000000
int get_free_page();
struct page_struct
{
    int used;
    char context[page_size - 4];
};

struct page_struct page[1000];
unsigned long PFN(unsigned long i);