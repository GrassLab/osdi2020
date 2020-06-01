#include "mm.h"
#include "schedule.h"
#include "string.h"
#include "uart.h"

// void map_page(int taskid, unsigned long va, unsigned long pageid) {
//   unsigned long pgd;
//   task_t *now = (task_t *)&task_pool[taskid];
//   if (now->mm.pgd) {
//     now->mm.pgd = &page[get_free_page()];
//     now->mm.kernel_pages[++now->mm.kernel_pages_count] = now->mm.pgd;
//   }
//   pgd = now->mm.pgd;
//   int new_table;
//   unsigned long pud =
//       map_table((unsigned long *)(pgd + VA_START), PGD_SHIFT, va,
//       &new_table);
//   if (new_table) {
//     now->mm.kernel_pages[++now->mm.kernel_pages_count] = pud;
//   }
//   unsigned long pmd =
//       map_table((unsigned long *)(pud + VA_START), PUD_SHIFT, va,
//       &new_table);
//   if (new_table) {
//     now->mm.kernel_pages[++now->mm.kernel_pages_count] = pmd;
//   }
//   unsigned long pte =
//       map_table((unsigned long *)(pmd + VA_START), PMD_SHIFT, va,
//       &new_table);
//   if (new_table) {
//     now->mm.kernel_pages[++now->mm.kernel_pages_count] = pte;
//   }
//   map_table_entry((unsigned long *)(pte + VA_START), va, pageid);
//   struct user_page p = {page, va};
//   now->mm.user_pages[now->mm.user_pages_count++] = p;
// }

int allocate_usr_page(int task_id, unsigned long va) {
  int getp = get_free_page();
  // map_page(task_id, va, getp);
  return getp;
}

unsigned long page_alloc() {
  unsigned long page_addr = (unsigned long)&page[get_free_page()].context;
  return page_addr;
}

unsigned long page_free(unsigned long sp) {
  unsigned long index = PFN(sp);
  if (page[index].used == 1) {
    page[index].used = 0;
    return 1;
  }
  return 0;
}

int get_free_page() {
  for (int i = 0; i < 1000; i++) {
    if (page[i].used == 0) {
      page[i].used = 1;
      for (unsigned long i = 0; i < page_size - 4; i++) {
        page[i].context[i] = 0;
      }
      return i;
    }
  }
  return 0;
}
unsigned long PFN(unsigned long i) {
  unsigned long in = i;
  printf("intput = 0x%x\n", in);
  in = in - (unsigned long)(&page[0]);
  printf("intput = 0x%x\n", in);
  in = (in & 0x0000fffffffff000) >> (4 * 3);
  printf("out = 0x%x\n", in);
  return in;
}