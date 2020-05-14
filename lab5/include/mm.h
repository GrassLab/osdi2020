#pragma once



#define VA_START            0xffff000000000000
#define DEVICE_BASE         0x3F000000
#define PBASE               DEVICE_BASE/* (VA_START + DEVICE_BASE) */

#define PAGE_SHIFT          12
#define TABLE_SHIFT         9
#define SECTION_SHIFT       (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE           (1 << PAGE_SHIFT)
#define SECTION_SIZE        (1 << SECTION_SHIFT)

#define LOW_MEMORY          (2 * SECTION_SIZE)
#define HIGH_MEMORY         PBASE

#define PAGING_MEMORY       (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES        (PAGING_MEMORY/PAGE_SIZE)

#define KERNEL_END_INDEX    ((PAGING_MEMORY >> 1) / PAGE_SIZE)


/* MMU page table setup related */
#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)

#define PHYS_MEMORY_SIZE        0x40000000

#define PAGING_MEMORY           (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES            (PAGING_MEMORY/PAGE_SIZE)

#define PTRS_PER_TABLE			(1 << TABLE_SHIFT)

#define PGD_SHIFT			PAGE_SHIFT + 3*TABLE_SHIFT
#define PUD_SHIFT			PAGE_SHIFT + 2*TABLE_SHIFT
#define PMD_SHIFT			PAGE_SHIFT + TABLE_SHIFT

#define PG_DIR_SIZE			(3 * PAGE_SIZE)

/* MMU */
#define MM_TYPE_PAGE_TABLE		0x3
#define MM_TYPE_PAGE            0x3
#define MM_TYPE_BLOCK			0x1
#define MM_ACCESS			(0x1 << 10)
#define MM_ACCESS_PERMISSION		(0x01 << 6)

#define MT_DEVICE_nGnRnE        0x0
#define MT_NORMAL_NC			0x1
#define MT_DEVICE_nGnRnE_FLAGS		0x00
#define MT_NORMAL_NC_FLAGS          0x44
#define MAIR_VALUE			(MT_DEVICE_nGnRnE_FLAGS << (8 * MT_DEVICE_nGnRnE)) | (MT_NORMAL_NC_FLAGS << (8 * MT_NORMAL_NC))

#define MMU_FLAGS           (MM_TYPE_BLOCK | (MT_NORMAL_NC << 2) | MM_ACCESS)
#define MMU_DEVICE_FLAGS		(MM_TYPE_BLOCK | (MT_DEVICE_nGnRnE << 2) | MM_ACCESS)
#define MMU_PTE_FLAGS			(MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS | MM_ACCESS_PERMISSION)



#ifndef __ASSEMBLER__

unsigned long get_free_page();
unsigned long get_user_free_page();
void free_page(unsigned long p);
void memzero(unsigned long src, unsigned long n);

#endif
