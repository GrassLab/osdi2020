#include "kernel/base.h"
#include "kernel/mm.h"
#include "kernel/lib/string.h"

extern char __kernel_start[];
extern char __kernel_end[];
struct page pages[PAGE_NUM];

void page_init(void) {
  for (uint64_t pfn = KVIRT_TO_PFN((uint64_t)__kernel_start); pfn <= KVIRT_TO_PFN((uint64_t)__kernel_end); ++pfn) {
    pages[pfn].inuse = true;
  }
}

/*
 * Allocate page for both kernel and user space, always clear with zero.
 * Return physical address of the allocated page.
 */
uint64_t page_alloc(void) {
  for (uint64_t i = 0; i < PAGE_NUM; ++i) {
    if (!pages[i].inuse) {
      pages[i].inuse = true;
      uint64_t frame = (i << 12);
      memset((void *)(KERNEL_VA_BASE + (i << 12)), 0, PAGE_SIZE);
      return frame;
    }
  }
  return (uint64_t)-1;
}

void page_free(uint64_t page_addr) {
  pages[KVIRT_TO_PFN(page_addr)].inuse = false;
}

/*
 * Create virtual address space for a user program.
 * This includes building levels of page tables and copy the program contents.
 */
uint64_t *build_user_va(uint64_t binary_start, size_t binary_size) {
  uint64_t *pgd = (uint64_t *)PA_TO_KVA(page_alloc());
  /* Currently assume user virtual mapping always start from 0. */
  for (uint64_t offset = 0; offset < binary_size; offset += PAGE_SIZE) {
    void *frame = create_mapping(pgd, offset);
    memcpy(frame, (const void *)(binary_start + offset), PAGE_SIZE);
  }
  return pgd;
}

/* Create page tables and memory frame for a virtual page. */
void *create_mapping(uint64_t *pgd, uint64_t va) {
  if ((pgd[PGD_INDEX(va)] & PD_VALID) != 1) {
    pgd[PGD_INDEX(va)] = page_alloc() | USER_PGD_ATTR;
  }

  uint64_t *pud = (uint64_t *)GET_PUD(pgd, va);
  if ((pud[PUD_INDEX(va)] & PD_VALID) != 1) {
    pud[PUD_INDEX(va)] = page_alloc() | USER_PUD_ATTR;
  }

  uint64_t *pmd = (uint64_t *)GET_PMD(pud, va);
  if ((pmd[PMD_INDEX(va)] & PD_VALID) != 1) {
    pmd[PMD_INDEX(va)] = page_alloc() | USER_PMD_ATTR;
  }

  uint64_t *pte = (uint64_t *)GET_PTE(pmd, va);
  if ((pte[PTE_INDEX(va)] & PD_VALID) != 1) {
    pte[PTE_INDEX(va)] = page_alloc() | USER_PTE_NORMAL_ATTR;
  }

  return (void *)GET_FRAME(pte, va);
}