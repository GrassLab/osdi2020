#include "io.h"
#include "mm.h"
#include "allocator.h"

/* buddy system below */

Zone const buddy_zone = &(ZoneStr){
  .free_area = { [0 ... MAX_ORDER - 1] = {0, 0} }
};

void init_pages(){
#if zonealoc
  page_number = (HIGH_MEMORY - LOW_MEMORY) / (PAGE_SIZE);
  printf("need Pages[%d]" NEWLINE, page_number);
  low_memory = LOW_MEMORY + (sizeof(Page) * page_number);
  unsigned long mask = (1 << (MAX_ORDER + PAGE_SHIFT)) - 1;
  printf("mask %x" NEWLINE, mask);
  printf("new temp low %x" NEWLINE, low_memory);
  if(mask & low_memory){
    printf("rmtail low %x" NEWLINE, (~mask & low_memory));
    printf("align samll %x" NEWLINE, (1 << (MAX_ORDER + PAGE_SHIFT - 1)));
    low_memory = (~mask & low_memory) + (1 << (MAX_ORDER + PAGE_SHIFT - 1));
  }
  page_number = (HIGH_MEMORY - low_memory) / (PAGE_SIZE);
  printf("final Pages[%d]" NEWLINE, page_number);
  /* VA_START is imporant */
  low_memory |= VA_START;
  printf("new aligned low 0x%x" NEWLINE, low_memory);
  /* VA_START is imporant */
  mpages = (Page*)(LOW_MEMORY | VA_START);
  for(int i = 0; i < page_number; i++)
    mpages[i].status = empty, mpages[i].order = 0;
#endif
}

Cdr newCdr(unsigned long addr, Cdr next){
  Cdr new;
  new = (Cdr)(addr);
  new->val = addr;
  new->cdr = next;
  return new;
}

void zone_init(Zone zone){
#if !zonealoc
  return;
#endif
  init_pages();
  unsigned size = page_number, order = MAX_ORDER - 1, block;
  unsigned long addr = ALOC_BEG;
  while(order){
    block = size / (1 << order);
    Cdr *iter = &(zone->free_area[order].free_list);
    while(block){
      zone->free_area[order].nr_free++;
      *iter = newCdr(addr, 0);
      iter = &((*iter)->cdr);
      addr += (1 << (order + PAGE_SHIFT));
      block--;
    }
    size = size % (1 << order);
    order--;
  }
  buddy_log("");
  buddy_log("inited buddy graph");
  buddy_log_graph(buddy_zone);
  buddy_log("");
}

void zone_show(Zone zone, unsigned long limit){
  for(int ord = 0; ord < MAX_ORDER; ord++){
    Cdr b = zone->free_area[ord].free_list;
    unsigned long nr = zone->free_area[ord].nr_free, cnt = limit;
    printfmt("order [%d] %d items", ord, nr);
    while(b && cnt){
      printfmt("  %s── %x to %x", b->cdr ? "├" : "└",
          b->val, b->val + (1 << (ord + PAGE_SHIFT)));
      b = b->cdr;
      cnt--;
      nr--;
    }
    if(nr) printfmt("  └ %s", "...");
  }
}

unsigned long zone_get_free_pages(Zone zone, int order){

  int ord = order;
  while(ord < MAX_ORDER && !zone->free_area[ord].free_list) ord++;

  if(ord == MAX_ORDER) goto buddy_allocate_failed;

  // partition to samller size
  while(ord > order){
    // 1st block
    Cdr block = zone->free_area[ord].free_list;
    zone->free_area[ord].free_list = block->cdr;
    zone->free_area[ord - 1].free_list = block;
    // 2nd block
    block->cdr = newCdr(block->val + (1 << (ord - 1 + PAGE_SHIFT)), 0);
    zone->free_area[ord].nr_free -= 1;
    zone->free_area[ord - 1].nr_free += 2;
    ord--;
  }

  if(!zone->free_area[ord].free_list)
    goto buddy_allocate_failed;

  // init pages
  Cdr block = zone->free_area[ord].free_list;
  zone->free_area[ord].free_list = block->cdr;
  zone->free_area[ord].nr_free -= 1;
  unsigned base = addr2pgidx(block->val);

  for (int i = 0; i < (1 << order); i++){
    mpages[base + i].status = used;
    unsigned long page = ALOC_BEG + (base + i) * PAGE_SIZE;
    memzero(page, PAGE_SIZE);
  }
  mpages[base].order = order;
  buddy_log("");
  buddy_log("allocate address 0x%x", ALOC_BEG + base * PAGE_SIZE);
  buddy_log_graph(buddy_zone);
  buddy_log("");
  return ALOC_BEG - VA_START + base * PAGE_SIZE;

buddy_allocate_failed:
  buddy_log("");
  buddy_log("buddy system allocate failed");
  buddy_log("");
  return 0;
}

void zone_merge_buddy(Zone zone, unsigned long addr, unsigned order){
  unsigned long mask = ((1 << (order + PAGE_SHIFT + 1)) - 1);
  unsigned long buddy_addr, aligned_addr;

  if(mask & addr){
    buddy_addr = (~mask) & addr;
    aligned_addr = buddy_addr;
  }
  else{
    buddy_addr = addr + (1 << (order + PAGE_SIZE));
    aligned_addr = addr;
  }

  Cdr *iter = &(zone->free_area[order].free_list);

  while(*iter){
    if((*iter)->val == buddy_addr){
      *iter = (*iter)->cdr;
      buddy_log("buddy found, merge order %d address %x", order + 1, aligned_addr);
      return zone_merge_buddy(zone, aligned_addr, order + 1);
    }
    iter = &((*iter)->cdr);
  }

  zone->free_area[order].free_list =
    newCdr(addr, zone->free_area[order].free_list);
}

void zone_free_pages(Zone zone, unsigned long addr){
  /* important */
  addr |= VA_START;
  unsigned base = addr2pgidx(addr);
  for(int i = 0; i < (1 << mpages[base].order); i++)
    mpages[base + i].status = empty;

  buddy_log("free address 0x%x", addr);
  buddy_log("merge order %d address 0x%x", mpages[base].order, addr);
  zone_merge_buddy(zone, addr, mpages[base].order);
  buddy_log("");
}

/* fixed size allocator below */

FixedAllocator fixed_allocator = 0;

unsigned long fixed_sized_get_token(unsigned long size){
  FixedAllocator *iter = &fixed_allocator, fa;
  while(*iter) iter = &((*iter)->next);
  unsigned long token = get_free_page();
  *iter = fa = (FixedAllocator)token;
  fa->size = size;
  fa->next = 0;
  unsigned book_nr = (PAGE_SIZE - sizeof(FixedBookStr)) / sizeof(FixedBookStr);
  FixedBook *book_ptr = &(fa->book);
  for(unsigned i = 1; i <= book_nr; i++){
     *book_ptr = (FixedBook)(token + i * sizeof(FixedBookStr));
     (*book_ptr)->next = 0;
     (*book_ptr)->free_nr = PAGE_SIZE / size;
     (*book_ptr)->page_addr = 0;
     book_ptr = &((*book_ptr)->next);
  }
  return token;
}

unsigned long fixed_sized_alloc(unsigned long token){
  return 0;
}

unsigned long fixed_sized_free(unsigned long addr){
  return 0;
}
