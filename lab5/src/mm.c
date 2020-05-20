#include "mm.h"
#include "arm/mmu.h"
#include "printf.h"

int remain_page = PAGING_PAGES;

static unsigned short mem_map[PAGING_PAGES] = {
    0,
};

unsigned long physical2pfn(unsigned long p) { return p >> 12; }

/* reserve 1 memory page & return */
unsigned long allocate_kernel_page() {
  unsigned long page = get_free_page();
  if (page == 0) {
    return 0;
  }
  return page + VA_START;
}

/* reserve 1 memory page for task & map it to virtual address */
unsigned long allocate_user_page(struct task_struct *task, unsigned long va) {
  unsigned long page = get_free_page();
  if (page == 0) {
    return 0;
  }
  map_page(task, va, page);

  return page + VA_START;
}

unsigned long get_free_page() {
  for (int i = 0; i < PAGING_PAGES; i++) {
    if (mem_map[i] == 0) {
      --remain_page;
      mem_map[i] = 1;
      unsigned long page = LOW_MEMORY + i * PAGE_SIZE;
      memzero(page + VA_START, PAGE_SIZE);
      return page;
    }
  }
  return 0;
}

void free_page(unsigned long phy_addr) {
  unsigned long pfn = physical2pfn(phy_addr);
  if (mem_map[pfn]) {
    mem_map[pfn] = 0;
    ++remain_page;
  }

  /* mem_map[(phy_addr - LOW_MEMORY) / PAGE_SIZE] = 0; */
  /* ++remain_page; */
}

void map_table_entry(unsigned long *pte, unsigned long va, unsigned long pa) {
  unsigned long index = va >> PAGE_SHIFT;
  index &= (PTRS_PER_TABLE - 1);
  unsigned long entry = pa | MMU_PTE_FLAGS;
  pte[index] = entry;
}

/* table: parent page table */
/* shift: extract the table index from va */
/* new_table: output 1: allocated, 0: otherwise */
unsigned long map_table(unsigned long *table, unsigned long shift,
                        unsigned long va, unsigned long *new_table) {
  /* extract the index from va with giving shift offset */
  unsigned long index = va >> shift;
  index &= (PTRS_PER_TABLE - 1);

  /* check wheather the child table existed ? */
  /*   exist:      *new_table = 0 as output */
  /*   otherwise:  create next level table */
  if (!table[index]) {
    *new_table = 1;
    unsigned long next_level_table = get_free_page();
    unsigned long entry = next_level_table | MM_TYPE_PAGE_TABLE;
    table[index] = entry;
    return next_level_table;
  } else {
    *new_table = 0;
  }
  return table[index] & PAGE_MASK;
}

void map_page(struct task_struct *task, unsigned long va, unsigned long page) {
  /* reserve a PGD page */
  if (!task->mm.pgd) {
    task->mm.pgd = get_free_page();
    task->mm.kernel_pages[task->mm.kernel_pages_count++] = task->mm.pgd;
  }
  unsigned long pgd = task->mm.pgd;

  /* construct table for PUD PMD PTE */
  unsigned long parent_table = pgd;
  unsigned long new_table;
  for (int i = 0; i < 3; ++i) {
    unsigned long next = map_table((unsigned long *)(parent_table + VA_START),
                                   PGD_SHIFT, va, &new_table);
    if (new_table) {
      task->mm.kernel_pages[task->mm.kernel_pages_count++] = next;
    }
    parent_table = next;
  }

  map_table_entry((unsigned long *)(parent_table + VA_START), va, page);
  struct user_page p = {page, va};
  task->mm.user_pages[task->mm.user_pages_count++] = p;
}

int copy_virt_memory(struct task_struct *dst) {
  struct task_struct *src = current;
  /* copy whole user page */
  for (int i = 0; i < src->mm.user_pages_count; i++) {
    /* allocate user page for process dst. */
    unsigned long kernel_va =
        allocate_user_page(dst, src->mm.user_pages[i].virt_addr);
    if (kernel_va == 0) {
      return -1;
    }
    /* copy the user page from src to dst -> */
    memcpy(src->mm.user_pages[i].virt_addr, kernel_va, PAGE_SIZE);
  }
  return 0;
}

/* static int ind = 1; */

int do_mem_abort(unsigned long addr, unsigned long esr) {
  println("[mem abort] page fault @ 0x%X kill the process", addr);
  exit_process();
  return 0;

  /* unsigned long dfs = (esr & 0b111111); /\* 0x3F *\/ */
  /* if ((dfs & 0b111100) == 0b100) { */
  /*   unsigned long page = get_free_page(); */
  /*   if (page == 0) { */
  /*     println("[mem abort] cannot find available page"); */
  /*     return -1; */
  /*   } */
  /*   map_page(current, addr & PAGE_MASK, page); */
  /*   ind++; */
  /*   if (ind > 2) { */
  /*     println("[mem abort] double fault @ 0x%X", addr); */
  /*     return -1; */
  /*   } */
  /*   return 0; */
  /* } */

  /* println("[mem abort] page fault @ 0x%X kill the process", addr); */
  /* return -1; */
}

void stat_memory_usage() {
  println("[stat] memory usage: ");
  println("|   pgd: 0x%X", (unsigned long)current->mm.pgd);
  println("|_  user pages map: // { v -> p }");
  for (int i = 0; i < current->mm.user_pages_count; ++i) {
    println("|       [%d]  0x%X -> 0x%X ", i,
            current->mm.user_pages[i].virt_addr,
            current->mm.user_pages[i].phys_addr);
  }

  println("|_  kernel pages:");

  for (int i = 0; i < current->mm.kernel_pages_count; ++i) {
    println("        [%d]  0x%X", i, current->mm.kernel_pages[i]);
  }
}
