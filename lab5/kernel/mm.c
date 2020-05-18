#include "mm.h"
#include "sched.h"
#include "libc.h"

static unsigned short mem_map[PAGING_PAGES] = {
    0,
};

unsigned long get_free_page() {
  for (int i = 0; i < PAGING_PAGES; i++) {
    if (mem_map[i] == 0) {
      mem_map[i] = 1;
      unsigned long page = LOW_MEMORY + i * PAGE_SIZE;
      memzero(page + VA_START, PAGE_SIZE);
      return page;
    }
  }
  return 0;
}

void free_page(unsigned long p) { mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0; }

/* /\* allocate a memory with alignment 4KB *\/ */
/* unsigned long get_free_page() { */
/*   for (int i = 0; i < KERNEL_END_INDEX; i++) { */
/*     /\* for (int i = 0; i < PAGING_PAGES; i++){ *\/ */
/*     if (mem_map[i] == 0) { */
/*       mem_map[i] = 1; */
/*       return LOW_MEMORY + i * PAGE_SIZE + VA_START; */
/*     } */
/*   } */
/*   return 0; */
/* } */

unsigned long get_user_free_page() {
  for (int i = 0; i < KERNEL_END_INDEX; i++) {
    if (mem_map[i] == 0) {
      mem_map[i] = 1;
      return LOW_MEMORY + i * PAGE_SIZE;
    }
  }
  return 0;
}

void map_table_entry(unsigned long *pte, unsigned long va, unsigned long pa) {
  unsigned long index = va >> PAGE_SHIFT;
  index = index & (PTRS_PER_TABLE - 1);
  unsigned long entry = pa | MMU_PTE_FLAGS;
  pte[index] = entry;
}

unsigned long map_table(unsigned long *table, unsigned long shift,
                        unsigned long va, int *new_table) {
  /* extract va index for PGD, PUD or PMD */
  unsigned long index = va >> shift;
  index = index & (PTRS_PER_TABLE - 1);

  if (!table[index]) {
    *new_table = 1;
    unsigned long next_level_table = get_free_page();
    unsigned long entry = next_level_table | MM_TYPE_PAGE_TABLE;
    table[index] = entry;
    return next_level_table;
  }

  *new_table = 0;
  return table[index] & PAGE_MASK;
}

/* only map a single page */
void map_page(struct task_struct *task, unsigned long va, unsigned long page) {
  if (!task->mm.pgd) {
    /* allocate a pgd page for task */
    task->mm.pgd = get_free_page();
    /* [pgd, 0, ... , 0] : unsigned long with size kernel_pages_count */
    task->mm.kernel_pages[++task->mm.kernel_pages_count] = task->mm.pgd;
  }

  /* swith to the current task pgd */
  unsigned long pgd = task->mm.pgd;

  int new_table;
  unsigned long pud =
      map_table((unsigned long *)(pgd + VA_START), PGD_SHIFT, va, &new_table);
  if (new_table) {
    /* [pgd, pud, 0 ... , 0] : unsigned long with size kernel_pages_count */
    task->mm.kernel_pages[++task->mm.kernel_pages_count] = pud;
  }
  unsigned long pmd =
      map_table((unsigned long *)(pud + VA_START), PUD_SHIFT, va, &new_table);
  if (new_table) {
    /* [pgd, pud, pmd, 0 ... , 0] : unsigned long with size kernel_pages_count
     */
    task->mm.kernel_pages[++task->mm.kernel_pages_count] = pmd;
  }
  unsigned long pte =
      map_table((unsigned long *)(pmd + VA_START), PMD_SHIFT, va, &new_table);
  if (new_table) {
    /* [pgd, pud, pmd, pte, 0 ... , 0] : unsigned long with size
     * kernel_pages_count */
    task->mm.kernel_pages[++task->mm.kernel_pages_count] = pte;
  }

  map_table_entry((unsigned long *)(pte + VA_START), va, page);
  struct user_page p = {page, va};
  task->mm.user_pages[task->mm.user_pages_count++] = p;
}

unsigned long allocate_user_page(struct task_struct *task, unsigned long va) {
  unsigned long page = get_free_page();
  if (page == 0) {
    return 0;
  }
  map_page(task, va, page);

  stat_memory_usage();
  unsigned long res = page + VA_START;
  uart_println("[allocate] user page at 0x%x%x", res >> 32, res);
  return res;
}

unsigned long allocate_kernel_page() {
  unsigned long page = get_free_page();
  if (page == 0) {
    return 0;
  }
  unsigned long res = page + VA_START;
  uart_println("[allocate] kernel page at 0x%x%x", res >> 32, res);
  return res;
}

int copy_virt_memory(struct task_struct *dst) {
  struct task_struct *src = current;
  for (int i = 0; i < src->mm.user_pages_count; i++) {
    unsigned long kernel_va =
        allocate_user_page(dst, src->mm.user_pages[i].virt_addr);
    if (kernel_va == 0) {
      return -1;
    }
    memcpy(kernel_va, src->mm.user_pages[i].virt_addr, PAGE_SIZE);
  }

  return 0;
}

static int ind = 1;

int do_mem_abort(unsigned long addr, unsigned long esr) {
  unsigned long dfs = (esr & 0b111111);
  if ((dfs & 0b111100) == 0b100) {
    unsigned long page = get_free_page();
    if (page == 0) {
      return -1;
    }
    map_page(current, addr & PAGE_MASK, page);
    ind++;
    if (ind > 2) {
      return -1;
    }
    return 0;
  }
  return -1;
}

void stat_memory_usage() {
  uart_println("memory usage: ");
  struct mm_struct *m = &current->mm;
#include "libc.h"
  uart_println("  pgd: %x%x", (unsigned long)m->pgd >> 32, m->pgd);

  unsigned long u = m->user_pages_count, k = m->kernel_pages_count;

  uart_println("  user pages: ");
  for (int i = 0; i < u; ++i) {
    uart_println("  [%d]", i);
    uart_println("    p:%x%x", (unsigned long)m->user_pages[i].phys_addr >> 32,
                 m->user_pages[i].phys_addr);
    uart_println("    v:%x%x", (unsigned long)m->user_pages[i].virt_addr >> 32,
                 m->user_pages[i].virt_addr);
  }

  uart_println("  kernel pages: ");
  for (int i = 0; i < k; ++i) {
    uart_println("  [%d]", i);
    uart_println("    k:%x%x", (unsigned long)m->kernel_pages[i] >> 32, m->kernel_pages[i]);
  }

  uart_println("  user-page-count:   %d", u);
  uart_println("  kernel-page-count: %d", k);
}
