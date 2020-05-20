#include "kernel/base.h"
#include "kernel/mm.h"
#include "kernel/lib/string.h"

extern char __kernel_start[];
extern char __kernel_end[];
struct page pages[PAGE_NUM];
uint64_t free_page_nums;

void page_init(void) {
  free_page_nums = PAGE_NUM;
  for (uint64_t pfn = KVIRT_TO_PFN((uint64_t)__kernel_start); pfn <= KVIRT_TO_PFN((uint64_t)__kernel_end); ++pfn) {
    pages[pfn].inuse = true;
    --free_page_nums;
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
      --free_page_nums;
      uint64_t frame = (i << 12);
      memset((void *)(KERNEL_VA_BASE + (i << 12)), 0, PAGE_SIZE);
      return frame;
    }
  }
  return (uint64_t)-1;
}

void page_free(uint64_t page_addr) {
  pages[KVIRT_TO_PFN(page_addr)].inuse = false;
  ++free_page_nums;
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

void copy_vmmap(uint64_t *dst, uint64_t *src, uint8_t level) {
  for (size_t i = 0; i < 512; ++i) {
    if ((src[i] & PD_VALID) == 0) {
      continue;
    }

    switch (level) {
    case 1:
    case 2:
    case 3:
      dst[i] = page_alloc() | USER_PGD_PUD_PMD_ATTR;
      copy_vmmap(PTBENT_TO_KVA(dst[i]), PTBENT_TO_KVA(src[i]), level + 1);
      break;
    case 4:
      dst[i] = page_alloc() | USER_PTE_NORMAL_ATTR;
      memcpy(PTBENT_TO_KVA(dst[i]), PTBENT_TO_KVA(src[i]), PAGE_SIZE);
      break;
    }
  }
}

void reclaim_vmmap(uint64_t *ptb, uint8_t level) {
  for (size_t i = 0; i < 512; ++i) {
    if ((ptb[i] & PD_VALID) == 0) {
      continue;
    }

    switch (level) {
    case 1:
    case 2:
    case 3:
      reclaim_vmmap(PTBENT_TO_KVA(ptb[i]), level + 1);
      break;
    case 4:
      page_free((uint64_t)PTBENT_TO_KVA(ptb[i]));
      break;
    }
  }
  page_free((uint64_t)ptb);
}