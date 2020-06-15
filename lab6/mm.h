#define page_size (1 << 12)
#define VA_START 0xffff000000000000
#define NULL 0

unsigned long page_alloc();
int get_free_page(int order);
void init_buddy();
struct page_struct
{
	int used;
	int order;
	struct page_struct *next;
	char *pad;
};

char page[1024][page_size];
unsigned long PFN(unsigned long i);
unsigned long page_free(unsigned long sp);
struct page_struct page_use[1024];
struct page_struct page_order[11];

// order 0 1
// order 1 2
// order 2 4
// order 3 8
// order 4 16
// order 5 32
// order 6 64
// order 7 128
// order 8 256
// order 9 512
// order 10 1024
