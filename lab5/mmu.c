#include <stdint.h>
#include "mmu.h"

void mmu_ttbr0_ttbr1_el1_init(void)
{
  /* ttbr0_el1: Holds the base address of the translation table for the initial lookup for stage 1 of the translation of an address from the lower VA range in the EL1&0 translation regime, and other information for this translation regime. */

  /* PD (ttbrx) -> PGD -> PUD -> PMD -> PTE -> | Page + offset */
  /*              47-38  38-29  29-20  20-11 */
  /*              512GB   1GB    2MB    4KB */


#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1

#define PAGE_SIZE (1 << 21) /* 2mb */

#define PD_TABLE 0x3 // indicate this to another descriptor
#define PD_BLOCK 0x1 // indicate to physical ram
#define PD_ACCESS (1 << 10) // access flag, generate page fault if not set
/* Set access bit and mair[4:2] and indicate to physical ram */
#define PD_DEVICE (MAIR_IDX_DEVICE_nGnRnE << 2)
#define PD_NORMAL (MAIR_IDX_NORMAL_NOCACHE << 2)

#define PGD_FRAME_BASE ((uint64_t *)0x0000u)
#define PUD_FRAME_BASE ((uint64_t *)0x1000u)
#define PMD_FRAME_BASE ((uint64_t *)0x2000u)

  /* setup pgd */
  uint64_t * pgd_frame_ptr = PGD_FRAME_BASE;
  /* combine the physical address of next level page with attribute. */
  *pgd_frame_ptr = (uint64_t)(PD_ACCESS | (uint64_t)PUD_FRAME_BASE | PD_TABLE);

  /* setup pud */
  uint64_t * pud_frame_ptr = PUD_FRAME_BASE;
  *pud_frame_ptr = (uint64_t)(PD_ACCESS | (uint64_t)PMD_FRAME_BASE | PD_TABLE);
  ++pud_frame_ptr;
  /* peripheral (virtual) 1GB */
  *pud_frame_ptr = (uint64_t)(PD_ACCESS | 0x40000000u | PD_BLOCK);

  /* setup pmd */
  /* 1GB = 512 * 2kb */
  uint64_t * pmd_frame_ptr = PMD_FRAME_BASE;
  for(unsigned i = 0; i < 512; ++i)
  {
    *(pmd_frame_ptr + i) = (uint64_t)(PD_ACCESS | (uint64_t)(PAGE_SIZE * i) | PD_NORMAL | PD_BLOCK);
  }

  /* ttbr0 and ttbr1 share the same PUD frame */
  asm volatile(
      "eor x0, x0, x0\n"
      "msr ttbr0_el1, x0\n" /* load PGD to the buttom translation based register. */
      "msr ttbr1_el1, x0\n" /* also load PGD to the upper translation based register. */
      );

  return;
}

