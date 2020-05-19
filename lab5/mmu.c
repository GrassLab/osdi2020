#include <stdint.h>
#include "string_util.h"
#include "meta_macro.h"
#include "mmu.h"
#include "uart.h"

/* 1gb = 2MB * 512 */
static struct page_struct mmu_page[PAGE_TOTAL];

static struct user_space_page_struct user_space_mm;

void mmu_ttbrx_el1_init(void)
{
  /* ttbr1_el1: Holds the base address of the translation table for the initial lookup for stage 1 of the translation of an address from the lower VA range in the EL1&0 translation regime, and other information for this translation regime. */

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
  for(unsigned i = 0; i < PAGE_TOTAL; ++i)
  {
    /* [TODO] Setup as normal, solve exception occurred when set page as normal */
    *(pmd_frame_ptr + i) = (uint64_t)(PD_ACCESS | (uint64_t)(PAGE_SIZE * i) | PD_DEVICE | PD_BLOCK);
  }

  /* setup ttbr1_el1 */
  asm volatile(
      "eor x0, x0, x0\n"
      "msr ttbr0_el1, x0\n" /* setup to prevent page fault when accessing phsycial ram [BUG OR FEATURE?] */
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

  for(unsigned page_idx = PAGE_PFN_LOW; page_idx < PAGE_PFN_HIGH; ++page_idx)
  {
    CLEAR_BIT(mmu_page[page_idx].flag, PAGE_USED);
  }

  for(unsigned page_idx = PAGE_PFN_HIGH; page_idx < PAGE_TOTAL; ++page_idx)
  {
    SET_BIT(mmu_page[page_idx].flag, PAGE_USED);
  }

  /* allocate page for user space page tables frame */
  user_space_mm.page_table_base = mmu_page_allocate();
  user_space_mm.page_frame_base = mmu_page_allocate();
  mmu_create_user_page_table_32KB();
  return;
}

uint64_t * mmu_page_allocate(void)
{
  for(unsigned current_pfn = PAGE_PFN_LOW; current_pfn < PAGE_PFN_HIGH; ++current_pfn)
  {
    if(!CHECK_BIT(mmu_page[current_pfn].flag, PAGE_USED))
    {
      uint64_t * va = (uint64_t *)MMU_PFN_TO_VA(current_pfn);
      memzero((char *)va, PAGE_SIZE);
      SET_BIT(mmu_page[current_pfn].flag, PAGE_USED);
      return va;
    }
  }
  return 0x0;
}

void mmu_page_free(uint64_t * va)
{

  CLEAR_BIT(mmu_page[MMU_VA_TO_PFN(((uint64_t)va))].flag, PAGE_USED);
  return;
}

void mmu_create_user_page_table_32KB(void)
{
  /* 2MB = 64 * 32kb */

  /* 2MB = 512 * 4kb, there are 64(TASK_POOL_SIZE) tasks in total */

  /* Share PGD, PUD. Each task has its own PTE, PMD points to the one on context switch */

  /* stack will be at 0x0000_ffff_ffff_e000 for all user task */

  /* 24kb for .text, 8kb for stack */

  /* page table frame */
  /*
   * offset     .text sp
   * 0x0000: PGD [0] [511]
   *              |    |
   * 0x1000: PUD [0] [511]
   *              |    |
   * 0x2000: PMD [0] [511]
   *   pmd_text_base, pmd_stack_base
   *              |    |
   *       based on user task id
   *              |             \
   *              |              \
   * 0x3000: pte_text_base[0]; 0x4000:pte_text_base[0]
   *         6 page descriptor[0:5]; 2 page descriptor[509:510]
   *
   * 0x5000: pte_text_base[1]; 0x6000:pte_text_base[1]
   *         6 page descriptor[0:5]; 2 page descriptor[509:510]
   */
  /* total frame: 1PGD, 1PUD, 1PMD, 64 * 2 PTE */

  /* setup 1 PGD */
  user_space_mm.pgd_base = user_space_mm.page_table_base;
  user_space_mm.pud_base = user_space_mm.pgd_base + 512;

  *user_space_mm.pgd_base = PD_ACCESS | ((uint64_t)(user_space_mm.pud_base)) | PD_TABLE;

  *(user_space_mm.pgd_base + 511) = PD_ACCESS | ((uint64_t)(user_space_mm.pud_base + 511)) | PD_TABLE;

  /* setup 1 PUD */
  user_space_mm.pmd_text_base = user_space_mm.pud_base + 512;
  user_space_mm.pmd_stack_base = user_space_mm.pmd_text_base + 511;

  *(user_space_mm.pud_base) = PD_ACCESS | ((uint64_t)(user_space_mm.pmd_text_base)) | PD_TABLE;
  *(user_space_mm.pud_base + 511) = PD_ACCESS | ((uint64_t)(user_space_mm.pmd_stack_base)) | PD_TABLE;

  /* PMD will be decide based on task id */

  /* setup PTE for all task */
  for(unsigned task_idx = 0; task_idx < TASK_POOL_SIZE; ++task_idx)
  {
    if(task_idx == 0)
    {
      user_space_mm.pte_text_base[0] = user_space_mm.pmd_text_base + 512;
      user_space_mm.pte_stack_base[0] = user_space_mm.pmd_text_base + 512 * 2;
    }
    else
    {
      user_space_mm.pte_text_base[task_idx] = user_space_mm.pte_text_base[task_idx - 1] + 512 * 2;
      user_space_mm.pte_stack_base[task_idx] = user_space_mm.pte_stack_base[task_idx - 1] + 512 * 2;
    }

    /* 32KB = 6(uint64_t) * 4KB + 2(uint64_t) * 4KB*/
    /* text & stack */
    user_space_mm.user_space_text_pa_base[task_idx] = user_space_mm.page_frame_base + task_idx * 512 * 8;

    /* setup pte_text_base, 7 descriptor */
    for(unsigned page_descriptor_idx = 0; page_descriptor_idx < 6; ++page_descriptor_idx)
    {
      *(user_space_mm.pte_text_base[task_idx] + page_descriptor_idx) = PD_ACCESS | ((uint64_t)(user_space_mm.user_space_text_pa_base[task_idx] + 512 * page_descriptor_idx)) | PD_USER_ACCESS | PD_NORMAL | PD_PTE_BLOCK;
    }

    /* setup pte_stack_base, 2 descriptor */
    *(user_space_mm.pte_stack_base[task_idx] + 509) = PD_ACCESS | ((uint64_t)(user_space_mm.user_space_text_pa_base[task_idx] + 512 * 6)) | PD_USER_ACCESS | PD_NORMAL | PD_PTE_BLOCK;
    *(user_space_mm.pte_stack_base[task_idx] + 510) = PD_ACCESS | ((uint64_t)(user_space_mm.user_space_text_pa_base[task_idx] + 512 * 7)) | PD_USER_ACCESS | PD_NORMAL | PD_PTE_BLOCK;
  }

  /* after this kernel cannot to access physical memory directly without add VA_BASE to address */
  asm volatile(
      "mov x0, %0\n"
      "msr ttbr0_el1, x0\n"
      : : "r"(user_space_mm.page_table_base));
  return;
}

uint64_t * mmu_user_task_set_pmu(uint64_t idx)
{
  *MMU_PA_TO_VA(user_space_mm.pmd_text_base) = PD_ACCESS | (uint64_t)user_space_mm.pte_text_base[idx] | PD_TABLE;
  *MMU_PA_TO_VA(user_space_mm.pmd_stack_base) = PD_ACCESS | (uint64_t)user_space_mm.pte_stack_base[idx] | PD_TABLE;
  asm volatile(
      "dsb ish\n"
      "tlbi vmalle1is\n"
      "dsb ish\n"
      "isb\n"
      );
  return MMU_PA_TO_VA(user_space_mm.user_space_text_pa_base[idx]);
}

