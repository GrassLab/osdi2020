#define HIGH_MEMORY 0x3F000000
#define LOW_MEMORY 0x600000 
#define PHYS_SIZE 0x40000000
#define PAGE_SIZE (0x1000) //4096
#define NUM_PAGE (PHYS_SIZE/PAGE_SIZE)
#define SECTION_SIZE (1<<21)
#define NON_PRESERV_PAGE ( (3 * SECTION_SIZE)/PAGE_SIZE )
#define PD_TABLE 0b11
#define MAIR_IDX_DEVICE_nGnRnE 0
#define PD_ACCESS (1 << 10)
#define BOOT_PTE_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_TABLE)
typedef struct page{
    char used;
} page;

void page_struct_init();
unsigned long long* page_alloc();
void page_free(unsigned long long vir);
unsigned long long user_paging();
void memcpy(unsigned long long* from, unsigned long long* to, unsigned long long size);