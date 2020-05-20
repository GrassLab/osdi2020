#ifndef	_MM_H
#define	_MM_H

#include "peripherals/base.h"

#define VA_START            0xffff000000000000

#define PHYS_MEMORY_SIZE        0x40000000

#define PAGE_MASK			0xfffffffffffff000
#define PAGE_SHIFT          12
#define TABLE_SHIFT             9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE               (1 << PAGE_SHIFT)
#define SECTION_SIZE			(1 << SECTION_SHIFT)

#define LOW_MEMORY                  (2 * SECTION_SIZE)
#define HIGH_MEMORY                 DEVICE_BASE

#define PAGING_MEMORY           (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES            (PAGING_MEMORY/PAGE_SIZE)

#define PTRS_PER_TABLE			(1 << TABLE_SHIFT)

#define PGD_SHIFT			PAGE_SHIFT + 3*TABLE_SHIFT
#define PUD_SHIFT			PAGE_SHIFT + 2*TABLE_SHIFT
#define PMD_SHIFT			PAGE_SHIFT + TABLE_SHIFT

#define PG_DIR_SIZE			(3 * PAGE_SIZE)

#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)


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

extern int remain_page;


unsigned long get_free_page();
void free_page(unsigned long p);
void map_page(struct task_struct *task, unsigned long va, unsigned long page);
void memzero(unsigned long src, unsigned long n);
void memcpy(unsigned long src, unsigned long dst, unsigned long n);
void stat_memory_usage();

int copy_virt_memory(struct task_struct *dst);
unsigned long allocate_kernel_page();
unsigned long allocate_user_page(struct task_struct *task, unsigned long va);

extern unsigned long pg_dir;

#endif

#endif  /*_MM_H */
