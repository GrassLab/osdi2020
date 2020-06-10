#include "io.h"
#include "mm.h"
#include "mmu.h"
#include "task.h"
#include "syscall.h"

extern Task *current_task;
#define DemandPaging 0
#undef PAGING_PAGES
#define PAGING_PAGES (1024 * 64)
Page mpages[PAGING_PAGES] = {[0 ... PAGING_PAGES - 1] = {empty} };

unsigned long vir2phy(unsigned long vir){
  unsigned long pfn = (vir << 16) >> 16;
  unsigned long offset = (vir << 52) >> 52;
  pfn = (pfn)>>PAGE_SHIFT;
  return pfn * PAGE_SIZE | offset;
}

unsigned long phy2pfn(unsigned long phy){
  return phy >> 12;
}

void mark_reserved_pages(unsigned long end){
  // most free because LOW_MEMORY HIGH_MEMORY protect
  printf("LOW_MEMORY: 0x%x " NEWLINE
      "HIGH_MEMORY: 0x%x" NEWLINE,
      LOW_MEMORY, HIGH_MEMORY);
  for (int i = 0; i < PAGING_PAGES; i++){
    unsigned long addr = LOW_MEMORY + i*PAGE_SIZE + VA_START;
    if(addr <= end || addr >= VA_START + HIGH_MEMORY){
      mpages[i].status = reserved;
      printf("page[%d] %x reserved." NEWLINE, i, addr);
    }
  }
}

unsigned long get_free_page()
{
  for (int i = 0; i < PAGING_PAGES; i++){
    if (mpages[i].status == empty){
      mpages[i].status = used;
      unsigned long page = LOW_MEMORY + i * PAGE_SIZE;
      memzero(page + VA_START, PAGE_SIZE);
      return page;
    }
  }
  printf("free page failed." NEWLINE);
  return 0;
}

unsigned long allocate_kernel_page() {
  if(current_task->mm.kernel_pages_count >= MAX_PROCESS_PAGES)
    return 0;
  unsigned long page = get_free_page();
  if (page == 0) {
    return 0;
  }
  return page + VA_START;
}

unsigned long allocate_user_page_with_attr(
    Task *task, unsigned long va, unsigned long attr) {
  if(current_task->mm.user_pages_count >= MAX_PROCESS_PAGES)
    return 0;

  unsigned long page = get_free_page();
  if (page == 0) {
    return 0;
  }
  //puts("get free page succ");
  map_page(task, va, page, attr);
  return page + VA_START;
}

unsigned long map_table(unsigned long *table, unsigned long shift, unsigned long va, int* new_table) {
  unsigned long index = va >> shift;
  index = index & (PTRS_PER_TABLE - 1);
  if (!table[index]){
    *new_table = 1;
    unsigned long next_level_table = get_free_page();
    unsigned long entry = next_level_table | PD_TABLE;
    table[index] = entry;
    return next_level_table;
  } else {
    *new_table = 0;
  }
  return table[index] & PAGE_MASK;
}

void map_table_entry(unsigned long *pte,
    unsigned long va, unsigned long pa, unsigned long attr) {
  unsigned long index = va >> PAGE_SHIFT;
  index = index & (PTRS_PER_TABLE - 1);
  unsigned long entry = pa | attr;
  pte[index] = entry;
}

int map_page(Task *task, unsigned long va, unsigned long page, unsigned long attr){
  if(current_task->mm.user_pages_count >= MAX_PROCESS_PAGES) return -1;
  unsigned long pgd;
  if (!task->mm.pgd) {
    task->mm.pgd = get_free_page();
    task->mm.kernel_pages[task->mm.kernel_pages_count++] = task->mm.pgd;
  }
  pgd = task->mm.pgd;
  int new_table;
  unsigned long pud = map_table((unsigned long *)(pgd + VA_START), PGD_SHIFT, va, &new_table);
  if (new_table) {
    task->mm.kernel_pages[task->mm.kernel_pages_count++] = pud;
  }
  unsigned long pmd = map_table((unsigned long *)(pud + VA_START) , PUD_SHIFT, va, &new_table);
  if (new_table) {
    task->mm.kernel_pages[task->mm.kernel_pages_count++] = pmd;
  }
  unsigned long pte = map_table((unsigned long *)(pmd + VA_START), PMD_SHIFT, va, &new_table);
  if (new_table) {
    task->mm.kernel_pages[task->mm.kernel_pages_count++] = pte;
  }
  map_table_entry((unsigned long *)(pte + VA_START), va, page, attr);
  struct user_page p = {page, va, attr};
  task->mm.user_pages[task->mm.user_pages_count++] = p;
  return 0;
}

void free_page(unsigned long p){
  mpages[(p - LOW_MEMORY) / PAGE_SIZE].status = empty;
}

void free_task_pages(Task *task){
  //printf("exist k pages: %d" NEWLINE, task->mm.kernel_pages_count);
  for(int i = 0; i < task->mm.kernel_pages_count; i++){
    //printf("free %x" NEWLINE, task->mm.kernel_pages[i]);
    free_page(task->mm.kernel_pages[i]);
  }

  //printf("exist u pages: %d" NEWLINE, task->mm.user_pages_count);
  for(int i = 0; i < task->mm.user_pages_count; i++){
    //printf("free %x" NEWLINE, task->mm.user_pages[i]);
    free_page(task->mm.user_pages[i].phys_addr);
  }
}

int copy_virt_memory(Task *dst) {
  Task* src = current_task;
  for (int i = 0; i < src->mm.user_pages_count; i++) {
    unsigned long kernel_va = allocate_user_page_with_attr(
        dst, src->mm.user_pages[i].virt_addr, src->mm.user_pages[i].attr);
    if( kernel_va == 0) {
      return -1;
    }
    memcpy(src->mm.user_pages[i].virt_addr, kernel_va, PAGE_SIZE);
  }
  return 0;
}

int do_mem_abort(unsigned long addr, unsigned long esr) {
  unsigned long dfs = (esr & 0b111111);
  if ((dfs & 0b111100) == 0b100) {
    if(addr < USER_MEM_LIMIT && DemandPaging){
      unsigned long page = get_free_page();
      if (page == 0) {
        puts("cannot allocate memory for user");
        current_task->status = zombie;
        return -1;
      }
      if(map_page(current_task, addr & PAGE_MASK, page, MMU_PTE_FLAGS)){
        current_task->status = zombie;
        printf("[MAP PAGE] segmentation fault [0x%x]" NEWLINE, addr);
        return -1;
      }
      else return 0;
    }
    current_task->status = zombie;
    printf("segmentation fault [0x%x]" NEWLINE, addr);
    return -1;
  }
  int WnR = esr & 0b1000000 ? 1 : 0;
  char msg[2][10] = {"reading", "writing"};
  printf("segmentation fault [%s on 0x%x]" NEWLINE, msg[WnR], addr);
  current_task->status = zombie;
  return -1;
}

int is_overlap_user_va(unsigned long addr, unsigned long len){
  struct mm_struct *mmp = &(current_task->mm);
  for(int i = 0; i < mmp->user_pages_count; i++)
    if((addr <= mmp->user_pages[i].virt_addr &&
          addr + len > mmp->user_pages[i].virt_addr) ||
        (mmp->user_pages[i].virt_addr <= addr &&
         mmp->user_pages[i].virt_addr + PAGE_SIZE > addr)){
      return 1;
    }
  return 0;
}

unsigned long find_empty_addr(unsigned long addr, unsigned long len){
  addr &= PAGE_MASK;
  while(is_overlap_user_va(addr, PAGE_SIZE)){
    addr += PAGE_SIZE;
  }
  if(addr >= USER_MEM_LIMIT) return (unsigned long)MAP_FAILED;
  return addr;
}

int is_page_aligned(unsigned long addr){
  return (~(PAGE_MASK) & addr) == 0;
}

void *mmap(void* addr, unsigned long len,
    int prot, int flags, int file_start, int file_offset){
  unsigned long va = (unsigned long)addr;
  if(va){
    if(is_overlap_user_va(va, len) || !is_page_aligned(va)){
      if(flags & MAP_FIXED) return MAP_FAILED;
      va = find_empty_addr(va, len);
    }
  }
  else{
    va = find_empty_addr(0x0, len);
  }

  /* cannot find proper address */
  if(va == (unsigned long)MAP_FAILED) return MAP_FAILED;

  unsigned long size = len, pw_size, page, attr = MMU_PTE_FLAGS;
  unsigned long pg_ptr = va, va_ptr = va;
  int pa_ptr = file_start;

  attr &= ~(1ul << 6);
  //if(prot & PROT_NONE){
  //  attr &= ~(1ul << 6);
  //}
  if(prot & PROT_READ){
    attr |= (1ul << 6);
    attr |= (1ul << 7);
  }
  if(prot & PROT_WRITE){
    attr |= (1ul << 6);
    attr &= ~(1ul << 7);
  }
  if(prot & PROT_EXEC){
    attr |= (1ul << 6);
    attr &= ~(1ul << 54);
  }


  if(!is_page_aligned(pg_ptr))
    pg_ptr &= PAGE_MASK;

  unsigned long mmap_val = pg_ptr;

  while(size){
    page = allocate_user_page_with_attr(current_task, pg_ptr, attr);
    if(!page){
      printf("mmap allocate code page failed" NEWLINE);
      return MAP_FAILED;
    }
    // not handle offset yet
    pw_size = pg_ptr + PAGE_SIZE - va_ptr;
    pw_size = pw_size > size ? size : pw_size;
    if(pa_ptr >= 0){
      memcpy(pa_ptr, page + va_ptr - pg_ptr, pw_size);
      pa_ptr += pw_size;
    }
    va_ptr += pw_size;
    size -= pw_size;
    pg_ptr += PAGE_SIZE;
  }
  return (void*)mmap_val;
}
