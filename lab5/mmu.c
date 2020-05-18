#include <stdint.h>
#include "meta_macro.h"
#include "mmu.h"

/* 1gb = 2MB * 512 */
static struct page_struct mmu_page[512];

void mmu_ttbr0_ttbr1_el1_init(void)
{
  /* ttbr0_el1: Holds the base address of the translation table for the initial lookup for stage 1 of the translation of an address from the lower VA range in the EL1&0 translation regime, and other information for this translation regime. */

  /* PD (ttbrx) -> PGD -> PUD -> PMD -> PTE -> | Page + offset */
  /*              47-38  38-29  29-20  20-11 */
  /*              512GB   1GB    2MB    4KB */

  /* setup pgd */
  uint64_t * pgd_frame_ptr = PGD_FRAME_BASE;
  /* combine the physical address of next level page with attribute. */
  *pgd_frame_ptr = (uint64_t)(PD_ACCESS | (uint64_t)PUD_FRAME_BASE | PD_TABLE);

  /* setup pud */
  uint64_t * pud_frame_ptr = PUD_FRAME_BASE;
  *pud_frame_ptr = (uint64_t)(PD_ACCESS | (uint64_t)PMD_FRAME_BASE | PD_TABLE);
  ++pud_frame_ptr;
  /* peripheral (virtual) 1GB */
  *pud_frame_ptr = (uint64_t)(PD_ACCESS | 0x40000000u | PD_DEVICE | PD_BLOCK);

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

void mmu_page_init(void)
{
  /* each page is 2MB 0x200000 */
  /* PGD, PUD, PMD is occupy 0x0 - 0x2FFF */
  /* initial kernel stack 0x8000 */
  /* kernel8.img loads at 0x80000, kernel size is approx. 30KB+ */
  SET_BIT(mmu_page[0].flag, PAGE_USED);

  /* peripheral(0x3F000000 - 0x3FFFFFFF) */
  /* 0x3F000000 / 0x200000 = 504 */
  /* the rest are unused */

  for(unsigned page_idx = 1; page_idx < 504; ++page_idx)
  {
    CLEAR_BIT(mmu_page[page_idx].flag, PAGE_USED);
  }

  for(unsigned page_idx = 504; page_idx < 512; ++page_idx)
  {
    SET_BIT(mmu_page[page_idx].flag, PAGE_USED);
  }
}

