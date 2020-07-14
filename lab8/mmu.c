#include <stdint.h>
#include "string_util.h"
#include "meta_macro.h"
#include "mmu.h"
#include "buddy.h"
#include "uart.h"
#include "slab.h"

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
  /* 1GB = 512 * 2mb */
  uint64_t * pmd_frame_ptr = PMD_FRAME_BASE;
  for(unsigned i = 0; i < 504; ++i)
  {
    *(pmd_frame_ptr + i) = (uint64_t)(PD_ACCESS | (uint64_t)(PAGE_SIZE * i) | PD_NORMAL | PD_BLOCK);
  }
  /* 3f000000 gpu peripheral should be set to device, 16MB, 2MB * 8 */
  for(unsigned i = 504; i < 512; ++i)
  {
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

void mmu_startup_page_init(void)
{
  /* Setup startup page which is located at STARTUP_PAGE_TABLE_BASE + 1KB(0x400) */
  /* 1KB = 0x80 * 64bit */
  for(unsigned startup_page_idx = 0; startup_page_idx < STARTUP_PAGE_TOTAL; ++startup_page_idx)
  {
    CLEAR_BIT(((((struct page_struct *)MMU_PA_TO_VA(STARTUP_PAGE_TABLE_BASE)) + startup_page_idx) -> flag), PAGE_USED);
  }
  uart_puts(ANSI_MAGENTA"[Startup page allocator]"ANSI_RESET" Init complete\n");
}


uint64_t * mmu_startup_page_allocate(int zero)
{
  struct page_struct * startup_page_table_base = (struct page_struct *)MMU_PA_TO_VA(STARTUP_PAGE_TABLE_BASE);

  for(unsigned current_pfn = 0; current_pfn < STARTUP_PAGE_TOTAL; ++current_pfn)
  {
    if(!CHECK_BIT(startup_page_table_base[current_pfn].flag, PAGE_USED))
    {
      /* uint64_t is 8 byte */
      uint64_t * pa = (uint64_t *)(uint64_t)(STARTUP_PAGE_PA_BASE + current_pfn * PAGE_4K);
      uint64_t * va = MMU_PA_TO_VA(pa);
      if(zero)
      {
        memzero_8byte(va, PAGE_4K / 8);
      }
      SET_BIT(startup_page_table_base[current_pfn].flag, PAGE_USED);

#ifdef DEBUG
      /* print allocation message */
      char string_buff[0x20];

      uart_puts(ANSI_MAGENTA"[Startup page allocator]"ANSI_RESET" Allocate: ");
      string_ulonglong_to_hex_char(string_buff, (unsigned long long)va);
      uart_puts(string_buff);
      uart_puts("\n");
#endif
      return pa;
    }
  }
  return 0x0;
}

void mmu_startup_page_free(uint64_t * va)
{
  struct page_struct * startup_page_table_base = (struct page_struct *)MMU_PA_TO_VA(STARTUP_PAGE_TABLE_BASE);

  CLEAR_BIT(startup_page_table_base[MMU_VA_TO_PFN(((uint64_t)va))].flag, PAGE_USED);

#ifdef DEBUG
  /* free message */
  char string_buff[0x20];
  uart_puts(ANSI_MAGENTA"[Startup page allocator] "ANSI_RESET"Free: ");
  string_ulonglong_to_hex_char(string_buff, (unsigned long long)va);
  uart_puts(string_buff);
  uart_puts("\n");
#endif
  return;
}

void mmu_user_page_table_init(void)
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
  user_space_mm.pgd_base = buddy_allocate(0, 1, BUDDY_ALLOCATE_TO_PA);
  user_space_mm.pud_base = buddy_allocate(0, 1, BUDDY_ALLOCATE_TO_PA);

  *user_space_mm.pgd_base = PD_ACCESS | ((uint64_t)(user_space_mm.pud_base)) | PD_TABLE;

  *(user_space_mm.pgd_base + 511) = PD_ACCESS | ((uint64_t)(user_space_mm.pud_base + 511)) | PD_TABLE;

  /* setup 1 PUD */
  user_space_mm.pmd_text_base = buddy_allocate(0, 1, BUDDY_ALLOCATE_TO_PA);
  user_space_mm.pmd_stack_base = user_space_mm.pmd_text_base + 511;

  *(user_space_mm.pud_base) = PD_ACCESS | ((uint64_t)(user_space_mm.pmd_text_base)) | PD_TABLE;
  *(user_space_mm.pud_base + 511) = PD_ACCESS | ((uint64_t)(user_space_mm.pmd_stack_base)) | PD_TABLE;

  /* PMD and PTE will be decide based on task id */

  asm volatile(
      "mov x0, %0\n"
      "msr ttbr0_el1, x0\n"
      : : "r"(user_space_mm.pgd_base));
  return;
}

void mmu_create_user_pmd_pte(struct user_space_mm_struct * mm_struct)
{
  /* 24kb for .text, 8kb for stack */

  /* setup PTE for .text */
  mm_struct -> pte_text_base = buddy_allocate(0, 1, BUDDY_ALLOCATE_TO_PA);
  mm_struct -> pte_stack_base = buddy_allocate(0, 1, BUDDY_ALLOCATE_TO_PA);

  for(unsigned pd_idx = 0; pd_idx < 6; ++pd_idx)
  {
    *(MMU_PA_TO_VA(((mm_struct -> pte_text_base) + pd_idx))) = (uint64_t)buddy_allocate(0, 0, BUDDY_ALLOCATE_TO_PA) | PD_ACCESS | PD_USER_ACCESS | PD_NORMAL | PD_PTE_BLOCK;
  }

  *(MMU_PA_TO_VA(((mm_struct -> pte_stack_base) + 509u))) = (uint64_t)buddy_allocate(0, 0, BUDDY_ALLOCATE_TO_PA) | PD_ACCESS | PD_USER_ACCESS | PD_NORMAL | PD_PTE_BLOCK;
  *(MMU_PA_TO_VA(((mm_struct -> pte_stack_base) + 510u))) = (uint64_t)buddy_allocate(0, 0, BUDDY_ALLOCATE_TO_PA) | PD_ACCESS | PD_USER_ACCESS | PD_NORMAL | PD_PTE_BLOCK;

#ifdef DEBUG
  char string_buff[0x80];
  string_ulonglong_to_hex_char(string_buff, (uint64_t)(mm_struct -> pte_text_base));
  uart_puts("Text PTE: ");
  uart_puts(string_buff);
  string_ulonglong_to_hex_char(string_buff, (uint64_t)(mm_struct -> pte_stack_base));
  uart_puts("\nStack PTE: ");
  uart_puts(string_buff);
  uart_puts("\nText PTE 0 ~ 6:\n");
  for(unsigned pd_idx = 0; pd_idx < 6; ++pd_idx)
  {
    string_ulonglong_to_hex_char(string_buff, *(MMU_PA_TO_VA(((mm_struct -> pte_text_base) + pd_idx))));
    uart_puts(string_buff);
    uart_putc(' ');
  }
  uart_puts("\nStack PTE 509, 510:\n");
  string_ulonglong_to_hex_char(string_buff, *(MMU_PA_TO_VA(((mm_struct -> pte_stack_base) + 509u))));
  uart_puts(string_buff);
  uart_putc(' ');
  string_ulonglong_to_hex_char(string_buff, *(MMU_PA_TO_VA(((mm_struct -> pte_stack_base) + 510u))));
  uart_puts(string_buff);
  uart_putc('\n');
#endif
}

void mmu_user_task_set_pmd(struct user_space_mm_struct * mm_struct)
{
  *MMU_PA_TO_VA(user_space_mm.pmd_text_base) = PD_ACCESS | (uint64_t)(mm_struct -> pte_text_base) | PD_TABLE;
  *MMU_PA_TO_VA(user_space_mm.pmd_stack_base) = PD_ACCESS | (uint64_t)(mm_struct -> pte_stack_base) | PD_TABLE;
  asm volatile(
      "dsb ish\n"
      "tlbi vmalle1is\n"
      "dsb ish\n"
      "isb\n"
      );

  return;
}

void mmu_copy_user_to_text(char * src, struct user_space_mm_struct * dst_mm_struct, unsigned size)
{
  unsigned current_size = size;
  for(unsigned pd_idx = 0; pd_idx < 6; ++pd_idx)
  {
    char * dst = (char *)MMU_PA_TO_VA(((uint64_t)(*MMU_PA_TO_VA(((dst_mm_struct -> pte_text_base) + pd_idx))) & MMU_ADDR_MASK));

    memcopy(src + (PAGE_4K * pd_idx), dst, (current_size / PAGE_4K) == 0 ? current_size : PAGE_4K);
    if(current_size < PAGE_4K)
    {
      break;
    }
    current_size -= PAGE_4K;
  }
  return;
}

void mmu_copy_user_text_stack(struct user_space_mm_struct * src_mm_struct, struct user_space_mm_struct * dst_mm_struct)
{
  /* copy .text */
  for(unsigned pd_idx = 0; pd_idx < 6; ++pd_idx)
  {
    memcopy((char *)MMU_PA_TO_VA(((uint64_t)(*MMU_PA_TO_VA(((src_mm_struct -> pte_text_base) + pd_idx))) & MMU_ADDR_MASK)),
            (char *)MMU_PA_TO_VA(((uint64_t)(*MMU_PA_TO_VA(((dst_mm_struct -> pte_text_base) + pd_idx))) & MMU_ADDR_MASK)),
            PAGE_4K);
  }
  /* copy stack */
  memcopy((char *)MMU_PA_TO_VA(((uint64_t)(*MMU_PA_TO_VA(((src_mm_struct -> pte_stack_base) + 509))) & MMU_ADDR_MASK)),
          (char *)MMU_PA_TO_VA(((uint64_t)(*MMU_PA_TO_VA(((dst_mm_struct -> pte_stack_base) + 509))) & MMU_ADDR_MASK)),
          PAGE_4K);
  memcopy((char *)MMU_PA_TO_VA(((uint64_t)(*MMU_PA_TO_VA(((src_mm_struct -> pte_stack_base) + 510))) & MMU_ADDR_MASK)),
          (char *)MMU_PA_TO_VA(((uint64_t)(*MMU_PA_TO_VA(((dst_mm_struct -> pte_stack_base) + 510))) & MMU_ADDR_MASK)),
          PAGE_4K);
  return;
}

void mmu_reclaim_user_pages(struct user_space_mm_struct * mm_struct)
{
  for(int pd_idx = 0; pd_idx < 6; ++pd_idx)
  {
    buddy_free(MMU_PA_TO_VA(((uint64_t)(*MMU_PA_TO_VA(((mm_struct -> pte_text_base) + pd_idx))) & MMU_ADDR_MASK)));
  }
  buddy_free(MMU_PA_TO_VA(((uint64_t)(*MMU_PA_TO_VA(((mm_struct -> pte_stack_base) + 509))) & MMU_ADDR_MASK)));
  buddy_free(MMU_PA_TO_VA(((uint64_t)(*MMU_PA_TO_VA(((mm_struct -> pte_stack_base) + 510))) & MMU_ADDR_MASK)));

  buddy_free(MMU_PA_TO_VA(mm_struct -> pte_text_base));
  buddy_free(MMU_PA_TO_VA(mm_struct -> pte_stack_base));

  return;
}

