#ifndef KERNEL_MM_H_
#define KERNEL_MM_H_

/*
 * T0SZ, bits [5:0]:   The size offset of the memory region addressed by TTBR0_EL1
 *                     The region size is 2^(64-T0SZ) bytes
 * T1SZ, bits [21:16]: The size offset of the memory region addressed by TTBR1_EL1
 *                     The region size is 2^(64-T1SZ) bytes
 * TG1, bits [15:14]:  Set page size for the TTBR1_EL0, 0b00 means 4KB
 * TG1, bits [31:30]:  Set page size for the TTBR1_EL1, 0b10 means 4KB
 *
 * https://developer.arm.com/docs/ddi0595/b/aarch64-system-registers/tcr_el1
 */
#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB ((0b00 << 14) | (0b10 << 30))
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1

#define PD_VALID 1
#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_USER (1 << 6)
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR PD_TABLE
#define BOOT_PMD_ATTR PD_TABLE
#define BOOT_PTE_NORMAL_ATTR (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | 0b11)
#define BOOT_PTE_DEVICE_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | 0b11)
#define USER_PGD_ATTR PD_TABLE
#define USER_PUD_ATTR PD_TABLE
#define USER_PMD_ATTR PD_TABLE
#define USER_PGD_PUD_PMD_ATTR PD_TABLE
#define USER_PTE_NORMAL_ATTR (PD_ACCESS | PD_USER | (MAIR_IDX_NORMAL_NOCACHE << 2) | 0b11)

#define PHYSICAL_MEM_SIZE (1 << 30)
#define PAGE_SIZE (1 << 12)
#define PAGE_NUM (PHYSICAL_MEM_SIZE / PAGE_SIZE)

/* Get index used for access each level of page table. */
#define PTB_INDEX_MASK ((1ULL << 9) - 1)
#define PGD_INDEX(va) ((va & (PTB_INDEX_MASK << 39)) >> 39)
#define PUD_INDEX(va) ((va & (PTB_INDEX_MASK << 30)) >> 30)
#define PMD_INDEX(va) ((va & (PTB_INDEX_MASK << 21)) >> 21)
#define PTE_INDEX(va) ((va & (PTB_INDEX_MASK << 12)) >> 12)

/* Get lower layer of page table from upper layer. */
#define GET_PUD(pgd, va) (((KERNEL_VA_BASE + pgd[PGD_INDEX((uint64_t)va)]) >> 12) << 12)
#define GET_PMD(pud, va) (((KERNEL_VA_BASE + pud[PUD_INDEX((uint64_t)va)]) >> 12) << 12)
#define GET_PTE(pmd, va) (((KERNEL_VA_BASE + pmd[PMD_INDEX((uint64_t)va)]) >> 12) << 12)
#define GET_FRAME(pte, va) (((KERNEL_VA_BASE + pte[PTE_INDEX((uint64_t)va)]) >> 12) << 12)

#define KVIRT_TO_PFN(addr) (uint64_t)((addr >> 12) & ((1ULL << 36) - 1))
#define PA_TO_KVA(pa) (pa + KERNEL_VA_BASE)
#define PTBENT_TO_KVA(ent) ((uint64_t *)(KERNEL_VA_BASE | ((uint64_t)ent >> 12 << 12)))

#define USER_STACK_VA_BASE 0x0000ffffffffe000
#define USER_STACK_VA_TOP (USER_STACK_VA_BASE - PAGE_SIZE)

#ifndef __ASSEMBLER__

#include "kernel/lib/types.h"

struct page {
  bool inuse;
};

extern uint64_t free_page_nums;

void page_init(void);
uint64_t page_alloc(void);
void page_free(uint64_t page_addr);

uint64_t *build_user_va(uint64_t binary_start, size_t binary_size);
void *create_mapping(uint64_t *pgd, uint64_t va);
void copy_vmmap(uint64_t *dst, uint64_t *src, uint8_t level);
void reclaim_vmmap(uint64_t *ptb, uint8_t level);

void page_fault_handler(void);

#endif // __ASSEMBLER__

#endif // KERNEL_MM_H_