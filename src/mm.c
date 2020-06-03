#include "mm.h"

unsigned short mem_map [ PAGING_PAGES ] = {0,};
struct page page_map[PAGE_MAP_SIZE];
int page_begin = 0x1000;

unsigned long get_free_page()
{
	for (int i = 0; i < PAGING_PAGES; i++){
		if (mem_map[i] == 0){
			mem_map[i] = 1;
			return LOW_MEMORY + i*PAGE_SIZE;
		}
	}
	return 0;
}

void free_page(unsigned long p){
	mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}

void tlb_init() {
  for (int num = 0; num < page_begin; num++) {
      page_occupied(num);
  }
  for (int num = page_begin; num < PAGE_MAP_SIZE; num++) {
      page_init(num);
  }
}

void page_occupied(int num) {
  // set kernal used pages
  struct page* p;
  p = &page_map[num];
  p->status = ALLOCATED;
  p->id = num;
}

void page_init(int num) {
  // set kernal used pages
  struct page* p;
  p = &page_map[num];
  p->status = FREE;
  p->id = num;
}

struct page* page_alloc() {
  struct page* p = -1;
  for (int num = page_begin; num < PAGE_MAP_SIZE; num++) {
    if(page_map[num].status == FREE) {
      p = &page_map[num];
      p->virtual_addr = num << 12 + 0xffff000000000000;
      p->status = ALLOCATED;
      uart_puts("virtual address: ");
      uart_hex(p->virtual_addr);
      uart_send('\n');
      break;
    }
  }
  return p;
}

void page_free(struct page* p) {
  page_init(p->id);
}

void page_mapping(struct task* task, struct page* user_page) {
  task->user_page = user_page->id;
  struct page* pgd_page = page_alloc();
  struct page* pud_page = page_alloc();
  struct page* pmd_page = page_alloc();
  struct page* pte_page = page_alloc();
  task->pages[task->pages_num++] = pgd_page->id;
  task->pages[task->pages_num++] = pud_page->id;
  task->pages[task->pages_num++] = pmd_page->id;
  task->pages[task->pages_num++] = pte_page->id;
  unsigned long* pgd = pgd_page->virtual_addr;
  unsigned long* pud = pud_page->virtual_addr;
  unsigned long* pmd = pmd_page->virtual_addr;
  unsigned long* pte = pte_page->virtual_addr;
  *pgd = (unsigned long)pud | PD_TABLE;
  *pud = (unsigned long)pmd | PD_TABLE;
  *pmd = (unsigned long)pte | PD_TABLE;
  *pte = (unsigned long)user_page->virtual_addr | PD_TABLE | PD_ACCESS;
  task->pgd = pgd;
}

void move_ttbr(unsigned long* pgd) { asm volatile("msr ttbr0_el1, x0"); }
