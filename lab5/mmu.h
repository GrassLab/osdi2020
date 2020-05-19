#ifndef __MMU_H__
#define __MMU_H__

/* Setup T0SZ[5:0] and T1SZ[21:16] */
/* The size offset of the memory region addressed by TTBRx_EL1 */
/* The region size is 2 ** (64-TxSZ) bytes. */
#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))

/* Setup TG0[15:14] and TG1[31:30] */
/* Granule size for TTBRx_EL1 */
/* TG0: */
/* 0b00 -> 4KB */
/* 0b01 -> 64KB */
/* 0b10 -> 16KB */
/* TG1: */
/* 0b01 -> 16KB */
/* 0b10 -> 4KB */
/* 0b11 -> 64KB */
#define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))

#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)


/*
Gathering (G/nG)
- Determines whether multiple accesses can be merged into a single bus transaction
- nG: Number/size of accesses on the bus = number/size of accesses in code
Reordering (R/nR)
- Determines whether accesses to the same device can be reordered
- nR: Accesses to the same IMPLEMENTATION DEFINED block size will appear on the bus in program order
Early Write Acknowledgement (E/nE)
- Indicates to the memory system whether a buffer can send acknowledgements
- nE: The response should come from the end slave, not buffering in the interconnect
*/
/*
Device memory: 0b0000dd00
0b00 | Device-nGnRnE memory
0b01 | Device-nGnRE memory
0b10 | Device-nGRE memory
0b11 | Device-GRE memory
*/
#define MAIR_DEVICE_nGnRnE 0b00000000

/*
Normal memory: 0booooiiii, (oooo != 0000 and iiii != 0000)

o:
0b0000	See encoding of Attr
0b00RW, RW not 0b00	Normal memory, Outer Write-Through Transient
0b0100	Normal memory, Outer Non-cacheable
0b01RW, RW not 0b00	Normal memory, Outer Write-Back Transient
0b10RW	Normal memory, Outer Write-Through Non-transient
0b11RW	Normal memory, Outer Write-Back Non-transient

i:
0b0000	See encoding of Attr
0b00RW, RW not 0b00	Normal memory, Inner Write-Through Transient
0b0100	Normal memory, Inner Non-cacheable
0b01RW, RW not 0b00	Normal memory, Inner Write-Back Transient
0b10RW	Normal memory, Inner Write-Through Non-transient
0b11RW	Normal memory, Inner Write-Back Non-transient

RW:
0b0	No Allocate
0b1	Allocate
*/
#define MAIR_NORMAL_NOCACHE 0b01000100

#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1

#ifndef __ASSEMBLER__

#include <stdint.h>
#include "task.h"

#define PAGE_SIZE (1 << 21) /* 2mb */
#define PAGE_4K (1 << 12)

#define PD_TABLE 0x3 // indicate this to another descriptor
#define PD_BLOCK 0x1 // indicate to physical ram
#define PD_PTE_BLOCK 0x3 // every entry in pte should point to physcial ram
#define PD_ACCESS (1 << 10) // access flag, generate page fault if not set
#define PD_USER_ACCESS (1 << 6) // user access flag
/* Set access bit and mair[4:2] and indicate to physical ram */
#define PD_DEVICE (MAIR_IDX_DEVICE_nGnRnE << 2)
#define PD_NORMAL (MAIR_IDX_NORMAL_NOCACHE << 2)

#define PGD_FRAME_BASE ((uint64_t *)0x0000u)
#define PUD_FRAME_BASE ((uint64_t *)0x1000u)
#define PMD_FRAME_BASE ((uint64_t *)0x2000u)

#define PAGE_TOTAL 512
#define PAGE_PFN_LOW 1
#define PAGE_PFN_HIGH 504 /* exclusive, for loop */

#define PAGE_USED 0

#define USER_STACK_VA 0x0000ffffffffe000

/* select [29:21] */
#define MMU_VA_TO_PFN(va) ((va & 0x3fe00000uLL) >> 21)
#define MMU_VA_TO_PA(va) (va & 0x3fffffff)
#define MMU_PFN_TO_VA(pfn) (((uint64_t)pfn) << 21)
#define MMU_PA_TO_VA(pa) ((uint64_t *)((uint64_t)pa | 0xffff000000000000))
#define MMU_VA_TO_USER_VA(va) ((uint64_t *)((uint64_t)va & 0x0000ffffffffffff))

struct page_struct
{
  /* bit 0 -> 1 if used, 0 if unused */
  uint64_t flag;
};

struct user_space_page_struct
{
  uint64_t * page_table_base;
  uint64_t * page_frame_base;

  uint64_t * pgd_base;

  uint64_t * pud_base;

  uint64_t * pmd_text_base;
  uint64_t * pmd_stack_base;
  uint64_t * pte_text_base[TASK_POOL_SIZE];
  uint64_t * pte_stack_base[TASK_POOL_SIZE];
  uint64_t * user_space_text_pa_base[TASK_POOL_SIZE];
};

void mmu_ttbrx_el1_init(void);
void mmu_page_init(void);
uint64_t * mmu_page_allocate(void);
void mmu_page_free(uint64_t * va);
void mmu_create_user_page_table_32KB(void);
uint64_t * mmu_user_task_set_pmu(uint64_t idx);

#endif /* __ASSEMBLER__ */

#endif /* __MMU_H__ */

