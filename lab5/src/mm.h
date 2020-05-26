#include "sched.h"
#ifndef _MM_H
#define _MM_H


#define VA_START            0xffff000000000000

#define PHYS_MEMORY_SIZE    0x40000000  

#define PAGE_MASK           0xfffffffffffff000

#define PAGE_SHIFT          12
#define TABLE_SHIFT         9
#define SECTION_SHIFT       (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE           (1 << PAGE_SHIFT)   
#define SECTION_SIZE        (1 << SECTION_SHIFT)    

#define LOW_MEMORY          (2 * SECTION_SIZE)
#define HIGH_MEMORY         0x3F000000

#define INITUSED            (LOW_MEMORY/PAGE_SIZE)
#define PAGING_MEMORY       (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES        (PAGING_MEMORY/PAGE_SIZE)

#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB          ((0b00 << 14) |  (0b10 << 30))
#define TCR_CONFIG_DEFAULT      (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

#define MAIR_DEVICE_nGnRnE  0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE  0
#define MAIR_IDX_NORMAL_NOCACHE 1

#define MM_TYPE_PAGE_TABLE      0x3
#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define PD_ACCESS_PERMISSION        (0x01 << 6)
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)

#define MMU_FLAGS           (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_BLOCK)
#define MMU_DEVICE_FLAGS    (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
#define MMU_PTE_FLAGS       (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_TABLE | PD_ACCESS_PERMISSION)

#define PTRS_PER_TABLE          (1 << TABLE_SHIFT)

#define PGD_SHIFT           PAGE_SHIFT + 3*TABLE_SHIFT
#define PUD_SHIFT           PAGE_SHIFT + 2*TABLE_SHIFT
#define PMD_SHIFT           PAGE_SHIFT + TABLE_SHIFT

#define PG_DIR_SIZE         (3 * PAGE_SIZE)

#define NUMPAGES 1000

#ifndef __ASSEMBLER__
struct page_struct{
    int used;
};
struct page_struct pages[NUMPAGES];

unsigned long virtual_to_physical(unsigned long vir);
unsigned long physical_to_pfn(unsigned long phy);

unsigned long get_free_page();
void free_page(unsigned long p);
void fork_memcpy(void *dest, const void *src, unsigned long len);
void memzero(unsigned long src, unsigned long n);
void map_page(struct task *task, unsigned long va, unsigned long page);
unsigned long map_table(unsigned long *table, unsigned long shift, unsigned long va, int* new_table);
unsigned long allocate_kernel_page(); 
unsigned long allocate_user_page(struct task *task, unsigned long va); 
extern unsigned long pg_dir;
#endif

#endif  /*_MM_H */
