#pragma once

#define VA_START            0xffff000000000000
#define DEVICE_BASE         0x3F000000
/* #define PBASE               DEVICE_BASE//(VA_START + DEVICE_BASE) */

#define PAGE_MASK			0xfffffffffffff000
#define PAGE_SHIFT          12
#define TABLE_SHIFT         9
#define SECTION_SHIFT       (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE           (1 << PAGE_SHIFT)
#define SECTION_SIZE        (1 << SECTION_SHIFT)

#define LOW_MEMORY          (2 * SECTION_SIZE)
#define HIGH_MEMORY         DEVICE_BASE

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
/*
 * Memory region attributes:
 *
 *   n = AttrIndx[2:0]
 *			n	MAIR
 *   DEVICE_nGnRnE	000	00000000
 *   NORMAL_NC		001	01000100
 */
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

#define TCR_T0SZ			(64 - 48)
#define TCR_T1SZ			((64 - 48) << 16)
#define TCR_TG0_4K			(0 << 14)
#define TCR_TG1_4K			(2 << 30)
#define TCR_VALUE			(TCR_T0SZ | TCR_T1SZ | TCR_TG0_4K | TCR_TG1_4K)



#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1


#define LV3_PGD_ATTR PD_TABLE
#define LV3_PUD_ATTR PD_TABLE
#define LV3_PMD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
#define LV3_PMD_DEV_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
#define LV3_PMD_NOR_ATTR (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_BLOCK)

#define LV4_PGD_ATTR PD_TABLE
#define LV4_PUD_ATTR PD_TABLE
#define LV4_PMD_ATTR PD_TABLE
#define LV4_PTD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_PAGE)


#ifndef __ASSEMBLER__
#include "sched.h"

unsigned long get_free_page();
void free_page(unsigned long p);
void memzero(unsigned long src, unsigned long n);
unsigned long allocate_user_page(struct task_struct *task, unsigned long va);
unsigned long allocate_kernel_page();
int copy_virt_memory(struct task_struct *dst);
void stat_memory_usage();

extern void set_pgd(unsigned long pgd);
extern unsigned long get_pgd();

extern unsigned long pg_dir;

#endif
