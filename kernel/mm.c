#include "kernel/base.h"
#include "kernel/lib/ioutil.h"
#include "kernel/lib/string.h"
#include "kernel/mm.h"
#include "kernel/syscall.h"

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
    void *frame = vmmap_create(pgd, offset);
    memcpy(frame, (const void *)(binary_start + offset), PAGE_SIZE);
  }
  return pgd;
}

/* Create page tables and memory frame for a virtual page. */
void *vmmap_create(uint64_t *pgd, uint64_t va) {
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

void vmmap_copy(uint64_t *dst, uint64_t *src, uint8_t level) {
  for (size_t i = 0; i < 512; ++i) {
    if ((src[i] & PD_VALID) == 0) {
      continue;
    }

    switch (level) {
    case 1:
    case 2:
    case 3:
      dst[i] = page_alloc() | USER_PGD_PUD_PMD_ATTR;
      vmmap_copy(PTBENT_TO_KVA(dst[i]), PTBENT_TO_KVA(src[i]), level + 1);
      break;
    case 4:
      dst[i] = page_alloc() | USER_PTE_NORMAL_ATTR;
      memcpy(PTBENT_TO_KVA(dst[i]), PTBENT_TO_KVA(src[i]), PAGE_SIZE);
      break;
    }
  }
}

void vmmap_reclaim(uint64_t *ptb, uint8_t level) {
  for (size_t i = 0; i < 512; ++i) {
    if ((ptb[i] & PD_VALID) == 0) {
      continue;
    }

    switch (level) {
    case 1:
    case 2:
    case 3:
      vmmap_reclaim(PTBENT_TO_KVA(ptb[i]), level + 1);
      break;
    case 4:
      page_free((uint64_t)PTBENT_TO_KVA(ptb[i]));
      break;
    }
  }
  page_free((uint64_t)ptb);
}

void page_fault_handler(void) {
  uint64_t *faulting_va;
  asm volatile("mrs %0, far_el1" : "=r"(faulting_va));
  printk("task id: %u, page fault occured at %#x\n", do_get_taskid(), faulting_va);
  do_exit(-1);
}

void buddy_init(void) {
  uint64_t pfn_start = KVIRT_TO_PFN((uint64_t)__kernel_end);
  pfn_start = (pfn_start >> (MAX_ORDER - 1) << (MAX_ORDER - 1));
  uint64_t pfn_end = KVIRT_TO_PFN(0xffff00003f000000);

  for (uint64_t pfn = pfn_start; pfn < pfn_end; ++pfn) {
    pages[pfn] = (struct page){.inuse = false, .pfn = pfn, .order = -1};
    init_list_head(&pages[pfn].free_list);
  }

  for (int order = MAX_ORDER - 1; order >= 0; --order) {
    uint64_t pages_per_block = 1 << order;
    uint64_t block_nums = (pfn_end - pfn_start) / pages_per_block;

    init_list_head(&free_areas[order].free_list);
    for (uint64_t i = 0; i < block_nums; ++i, pfn_start += pages_per_block) {
      list_add_tail(&pages[pfn_start].free_list, &free_areas[order].free_list);
      free_areas[order].nr_free += 1;
    }
  }
}

struct page *buddy_alloc(int order) {
  if (order < 0 || order >= MAX_ORDER) {
    printk("[Error] buddy_alloc: Invalid order %u\n", order);
    return NULL;
  }

  int orig_order = order;
  for (; order < MAX_ORDER && list_empty(&free_areas[order].free_list); ++order) {}
  if (order >= MAX_ORDER) {
    printk("[Error] buddy_alloc: Can't allocate memory\n");
    return NULL;
  }

  struct page *victim = list_entry(list_del_head(&free_areas[order].free_list), struct page, free_list);
  victim->order = -1;
  printk("[Info] buddy_alloc: Allocate %#x\n", PFN_TO_KVIRT(victim->pfn));
  for (--order; order >= orig_order; --order) {
    struct page *buddy = victim + (1 << order);
    list_add_head(&buddy->free_list, &free_areas[order].free_list);
    buddy->order = order;
    printk("[Info] buddy_alloc: Release %#x to order %u\n", PFN_TO_KVIRT(buddy->pfn), order);
  }
  return victim;
}

void buddy_free(struct page *page, int order) {
  printk("[Info] buddy_free: Free %#x\n", PFN_TO_KVIRT(page->pfn));
  for (; order < MAX_ORDER; ++order) {
    page->order = order;
    struct page *buddy = &pages[page->pfn ^ (1 << order)];
    if (buddy->order == order) {
      printk("[Info] buddy_free: Merge %#x and %#x\n", PFN_TO_KVIRT(page->pfn), PFN_TO_KVIRT(buddy->pfn));
      list_del(&buddy->free_list);
      page = page < buddy ? page : buddy;
    } else {
      break;
    }
  }
  list_add_head(&page->free_list, &free_areas[order].free_list);
}