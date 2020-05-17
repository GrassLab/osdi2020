#include <stdint.h>
#include "mmu.h"

void mmu_ttbr0_ttbr1_el1_init(void)
{
  /* ttbr0_el1: Holds the base address of the translation table for the initial lookup for stage 1 of the translation of an address from the lower VA range in the EL1&0 translation regime, and other information for this translation regime. */

  /* PD (ttbrx) -> PGD -> PUD -> PMD -> | Page + offset */

#define MAIR_IDX_DEVICE_nGnRnE 0

#define PD_TABLE 0x3 // indicate this to another descriptor
#define PD_BLOCK 0x1 // indicate to physical ram
#define PD_ACCESS (1 << 10) // access flag, generate page fault if not set
#define BOOT_PGD_ATTR PD_TABLE
/* Set access bit and mair[4:2] and indicate to physical ram */
#define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)

#define PGD_FRAME_BASE ((uint64_t *)0x0000u)
#define PUD_FRAME_BASE ((uint64_t *)0x1000u)
#define PMD_FRAME_BASE ((uint64_t *)0x2000u)
#define PTE_FRAME_BASE ((uint64_t *)0x3000u)

  uint64_t * pgd_frame_ptr = PGD_FRAME_BASE;
  /* combine the physical address of next level page with attribute. */
  *pgd_frame_ptr = (uint64_t)(BOOT_PGD_ATTR | (uint64_t)PUD_FRAME_BASE);

  uint64_t * pud_frame_ptr = PUD_FRAME_BASE;
  *pud_frame_ptr = (uint64_t)(BOOT_PUD_ATTR | 0x0);
  ++pud_frame_ptr;
  *pud_frame_ptr = (uint64_t)(BOOT_PUD_ATTR | 0x40000000);

  /* ttbr0 and ttbr1 share the same PUD frame */
  asm volatile(
      "eor x0, x0, x0\n"
      "msr ttbr0_el1, x0\n" /* load PGD to the buttom translation based register. */
      "msr ttbr1_el1, x0\n" /* also load PGD to the upper translation based register. */
      );

  return;
}

