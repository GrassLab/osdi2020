#include "io.h"
#include "mm.h"
#include "irq.h"
#include "util.h"
#include "string.h"
#include "allocator.h"

/* buddy system below */

Zone buddy_zone = 0;

#define STARTUP_ARRAY_SIZE 4
unsigned startup_array_size = STARTUP_ARRAY_SIZE;
unsigned startup_cur = 0;
char startup_have_allocated = 0;
unsigned long *startup_addrs = (unsigned long []){[0 ... STARTUP_ARRAY_SIZE - 1] = 0};
unsigned long *startup_sizes = (unsigned long []){[0 ... STARTUP_ARRAY_SIZE - 1] = 0};

unsigned long search_addr(unsigned long addr, unsigned long size, unsigned long mask){
research:
  for(int i = 0; i < startup_cur; i++)
    if(overlap(addr, size, startup_addrs[i], startup_sizes[i])){
      addr = startup_addrs[i] + startup_sizes[i];
      if(mask & addr) addr = (~mask & addr) + (mask + 1);
      goto research;
    }
  return addr;
}

unsigned long startup_allocate(unsigned long size, unsigned long mask){
  unsigned long addr = LOW_MEMORY;
  if(startup_cur >= startup_array_size){
    // reallocate array dynamically
    unsigned long *addrs = (unsigned long*)kmalloc(sizeof(unsigned long) * (startup_array_size << 1));
    unsigned long *sizes = (unsigned long*)kmalloc(sizeof(unsigned long) * (startup_array_size << 1));
    for(int i = 0; i < startup_array_size; i++){
      addrs[i] = startup_addrs[i];
      sizes[i] = startup_sizes[i];
    }
    if(startup_have_allocated){
      kfree((unsigned long)startup_addrs);
      kfree((unsigned long)startup_sizes);
    }
    startup_addrs = addrs;
    startup_sizes = sizes;
    startup_array_size <<= 1;
    startup_have_allocated |= 1;
  }
  addr = search_addr(addr, size, mask);
  printfmt("allocate addr = %x end = %x", addr, addr + size);
  if(addr + size > HIGH_MEMORY) return 0;
  startup_addrs[startup_cur] = addr;
  startup_sizes[startup_cur] = size;
  startup_cur++;
  return addr;
}

unsigned long startup_max_cont_space_left(unsigned long mask){
  unsigned long beg = LOW_MEMORY, size = 0, end = HIGH_MEMORY;
  if(mask & beg) beg = (~mask & beg) + (mask + 1);
  for(int i = 0; i < startup_cur; i++)
    if(end > startup_addrs[i]) end = startup_addrs[i];
  size = end - beg;
  for(int i = 0; i < startup_cur; i++){
    end = HIGH_MEMORY;
    beg = startup_addrs[i] + startup_sizes[i];
    if(mask & beg) beg = (~mask & beg) + (mask + 1);
    printfmt("new beg %x", beg);
    for(int j = 0; j < startup_cur; j++){
      if(startup_addrs[j] > beg && end > startup_addrs[j])
        end = startup_addrs[j];
    }
    size = MAX(end - beg, size);
  }
  return size;
}

Cdr newCdr(unsigned long addr, Cdr next){
  Cdr new;
  new = (Cdr)(addr);
  new->val = addr;
  new->cdr = next;
  return new;
}

void zone_init(Zone zone){
  puts("zone init");
  unsigned long mask = (1 << (MAX_ORDER + PAGE_SHIFT - 1)) - 1, addr;
  unsigned rest_page = zone->page_number, order = MAX_ORDER - 1, block;
  addr = zone->addr = MM_START | startup_allocate(zone->page_number * PAGE_SIZE, mask);

  while(order){
    block = rest_page / (1 << order);
    Cdr *iter = &(zone->free_area[order].free_list);
    while(block){
      zone->free_area[order].nr_free++;
      *iter = newCdr(addr, 0);
      iter = &((*iter)->cdr);
      addr += (1 << (order + PAGE_SHIFT));
      block--;
    }
    rest_page = rest_page % (1 << order);
    order--;
  }
  buddy_log("");
  buddy_log("inited zone graph");
  buddy_log_graph(zone);
  buddy_log("");
}

void zone_show(Zone zone, unsigned limit){
  for(int ord = 0; ord < MAX_ORDER; ord++){
    Cdr b = zone->free_area[ord].free_list;
    unsigned nr = zone->free_area[ord].nr_free, cnt = limit;
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
  preempt_disable();
  int ord = order;
  while(ord < MAX_ORDER &&
      !zone->free_area[ord].free_list)
    ord++;

  if(ord == MAX_ORDER){
    puts(" == max order");
    goto buddy_allocate_failed;
  }

  // partition to samller size
  while(ord > order){
    // 1st block
    Cdr block = zone->free_area[ord].free_list;
    zone->free_area[ord].free_list = block->cdr;
    zone->free_area[ord - 1].free_list = block;
    // 2nd block
    block->cdr = newCdr(block->val + (1 << (ord - 1 + PAGE_SHIFT)), 0);
    if(zone->free_area[ord].nr_free == 0){
      printfmt("wrong 104 0x%x", zone->free_area[ord].free_list);
      while(1);
    }
    zone->free_area[ord].nr_free -= 1;
    zone->free_area[ord - 1].nr_free += 2;
    ord--;
  }

  if(!zone->free_area[ord].free_list){
    puts("no free list");
    goto buddy_allocate_failed;
  }

  // init pages
  Cdr block = zone->free_area[ord].free_list;
  zone->free_area[ord].free_list = block->cdr;
  if(zone->free_area[ord].nr_free == 0){
    puts("wrong 119");
    while(1);
  }
  zone->free_area[ord].nr_free -= 1;
  unsigned base = addr2pgidx(block->val, zone);

  for (int i = 0; i < (1 << order); i++){
    mpages[base + i].status = used;
    unsigned long page = zone->addr + (base + i) * PAGE_SIZE;
    memzero(page, PAGE_SIZE);
  }
  mpages[base].order = order;
  buddy_log("");
  buddy_log("allocate order %d, %d bytes", order, PAGE_SIZE << order);
  buddy_log("allocate address 0x%x", zone->addr + base * PAGE_SIZE);
  buddy_log_graph(buddy_zone);
  buddy_log("");
  preempt_enable();
  return zone->addr - VA_START + base * PAGE_SIZE;

buddy_allocate_failed:
  buddy_log("");
  buddy_log("buddy system allocate failed");
  buddy_log("");
  preempt_enable();
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
      zone->free_area[order].nr_free -= 1;
      return zone_merge_buddy(zone, aligned_addr, order + 1);
    }
    iter = &((*iter)->cdr);
  }

  zone->free_area[order].free_list =
    newCdr(addr, zone->free_area[order].free_list);
  zone->free_area[order].nr_free += 1;
}

void zone_free_pages(Zone zone, unsigned long addr){
  preempt_disable();
  /* important */
  addr |= VA_START;
  unsigned base = addr2pgidx(addr, zone);
  for(int i = 0; i < (1 << mpages[base].order); i++)
    mpages[base + i].status = empty;

  buddy_log("free address 0x%x", addr);
  buddy_log("merge order %d address 0x%x", mpages[base].order, addr);
  zone_merge_buddy(zone, addr, mpages[base].order);
  buddy_log("");
  preempt_enable();
}

/* fixed size allocator below */

FixedAllocator newFixedAllocator(unsigned long addr,
    unsigned long size, FixedBook book, FixedAllocator next){
  FixedAllocator new = (FixedAllocator)addr;
  new->size = size;
  new->rs = pow2roundup(size);
  new->next = next;
  new->book = book;
  return new;
}

FixedAllocator setFixedAllocator(FixedAllocator this,
    unsigned long size){
  this->size = size;
  this->rs = pow2roundup(size);
  return this;
}

FixedBook newFixedBook(FixedBook new,
    unsigned long page_addr, FixedBook next){
  new->page_addr = page_addr;
  new->next = next;
  new->free_nr = 0;
  new->table = 0;
  return new;
}

FixedBook prependFixedBooks(FixedBook next){
  unsigned long addr = get_free_page();
  unsigned nr = PAGE_SIZE / sizeof(FixedBookStr);
  FixedBook iter = (FixedBook)addr;
  for(unsigned i = 1; i < nr; i++)
    iter = newFixedBook(iter, 0, (FixedBook)(addr + i * sizeof(FixedBookStr)))->next;
  newFixedBook(iter, 0, next);
  return (FixedBook)addr;
}

unsigned long dispatch_tables(FixedBook book){
  unsigned long tables = get_free_page();
  unsigned nr = PAGE_SIZE / FixedBookTableSzie;
  for(int i = 0; i < nr && book && tables; i++, book = book->next)
    book->table = (char*)(tables + i * FixedBookTableSzie);
  return tables;
}

FixedAllocator fixed_allocator = 0;

int fixed_is_book_full(FixedAllocator aloctor, FixedBook bookiter){
  return (aloctor->rs > PAGE_SIZE && bookiter->free_nr)
    || (aloctor->rs <= PAGE_SIZE && bookiter->free_nr == (PAGE_SIZE / aloctor->rs));
}

FixedAllocator fixed_find_by_token(unsigned long token, FixedAllocator fixed){
  FixedAllocator aloctor = fixed;
  while(aloctor)
    if(aloctor == (FixedAllocator)token) break;
    else aloctor = aloctor->next;
  return aloctor;
}

void fixed_show(FixedAllocator fixed, unsigned limit){
  if(!fixed) return;
  printfmt("fixed 0x%x size %d (%d)",
      fixed, fixed->size, fixed->rs);
  FixedBook book = fixed->book;
  while(book){
    printfmt("  %s── book 0x%x free %d",
        book->next ? "├" : "└", book, book->free_nr);
    book = book->next;
  }
}

unsigned long fixed_get_token_by(unsigned long size, FixedAllocator *fixedptr){

  if(!fixedptr) return 0;

  disable_irq();
  if(!size) goto fixed_get_token_failed;

  FixedAllocator avail = *fixedptr;

  while(avail && avail->size) avail = avail->next;

  if(!avail){
    /* new allocators and books */
    unsigned long addr;
    if(!(addr = get_free_page()))
      goto fixed_get_token_failed;

    unsigned nr = PAGE_SIZE / sizeof(FixedAllocatorStr);
    for(unsigned i = 0; i < nr; i++)
      *fixedptr = newFixedAllocator(
          addr + i * sizeof(FixedAllocatorStr),
          0, 0, *fixedptr);

    avail = *fixedptr;
  }
  setFixedAllocator(avail, size);


  fixed_log("");
  fixed_log("fixed token %x size %d", avail, size);
  fixed_log_graph(avail);
  fixed_log("");
  enable_irq();
  return (unsigned long)avail;

fixed_get_token_failed:
  fixed_log("");
  fixed_log("fixed get token failed");
  fixed_log("");
  enable_irq();
  return 0;
}

#define free_token_check_exist 0

int fixed_free_token_by(FixedAllocator aloctor){

  if(!aloctor) return 1;

  disable_irq();

  FixedBook book = aloctor->book;

#if free_token_check_exist
  /* if any book is not full, return */
  while(book){
    if(!fixed_is_book_full(aloctor, book))
      goto fixed_free_token_failed;
    book = book->next;
  }
  book = aloctor->book;
#endif

  /* release all book memory */
  while(book){

    book->free_nr = 0;
    if(book->page_addr){
      fixed_log("free page 0x%x", book->page_addr);
      free_page(book->page_addr);
    }
    if(book->table){
      if(!((unsigned long)book->table & (~PAGE_MASK))){
        fixed_log("free page 0x%x", book->table);
        free_page((unsigned long)book->table);
      }
    }
    book = book->next;
  }
  aloctor->book = 0;
  aloctor->size = aloctor->rs = 0;

  fixed_log("");
  fixed_log("fixed free token %x", aloctor);
  fixed_log("");
  enable_irq();
  return 0;
#if free_token_check_exist
fixed_free_token_failed:
#endif
  fixed_log("");
  fixed_log("fixed free token failed");
  fixed_log("");
  enable_irq();
  return 1;
}

unsigned long fixed_alloc_by(FixedAllocator aloctor){

  if(!aloctor) return 0;

  disable_irq();

  FixedBook book = aloctor->book;
  while(book && book->page_addr && !(book->free_nr))
    book = book->next;

  if(!book) aloctor->book = book = prependFixedBooks(aloctor->book);

  if(!(book->page_addr)){
    if(aloctor->rs > PAGE_SIZE){
      unsigned ord = 0;
      while((PAGE_SIZE << ord) < aloctor->rs) ord++;
      book->page_addr = zone_get_free_pages(buddy_zone, ord);
      book->free_nr = 1;
    }
    else{
      book->page_addr = get_free_page();
      book->free_nr = PAGE_SIZE / aloctor->rs;
    }
  }

  if(!(book->page_addr))
    goto fixed_alloc_failed;

  if(!(book->table) && !dispatch_tables(book))
    goto fixed_alloc_failed;

  /* take an object from book */
  unsigned long offset = 0;
  while(btst(book->table, offset) && offset < PAGE_SIZE){
    offset += aloctor->rs;
  }


  bset(book->table, offset);
  book->free_nr -= 1;

  fixed_log("");
  fixed_log("fixed alloc %d bytes @0x%x by allocator %x",
      aloctor->size, book->page_addr + offset, aloctor);
  enable_irq();
  return book->page_addr + offset;

fixed_alloc_failed:
  fixed_log("");
  fixed_log("fixed allocate failed");
  fixed_log("");
  enable_irq();
  return 0;
}

int fixed_free_by(unsigned long addr, FixedAllocator aloctor, char test){

  if(!aloctor) return 0;

  disable_irq();

  FixedBook bookiter = aloctor->book;
  while(bookiter)
    if((bookiter->page_addr & PAGE_MASK) == (addr & PAGE_MASK))
      break;
    else bookiter = bookiter->next;

  if(!bookiter){
    if(test) goto fixed_free_test;
    else goto fixed_free_failed;
  }

  bclr(bookiter->table, (addr - bookiter->page_addr));
  bookiter->free_nr += 1;

  /* release the page */
  if(fixed_is_book_full(aloctor, bookiter)){
    fixed_log("free page 0x%x", bookiter->page_addr);
    free_page(bookiter->page_addr);
    bookiter->page_addr = 0;
  }

  fixed_log("");
  fixed_log("fixed free 0x%x", addr);
  enable_irq();
  return addr;

fixed_free_failed:
  fixed_log("");
  fixed_log("fixed free failed");
  fixed_log("");
  enable_irq();
fixed_free_test:
  return 0;
}

unsigned long fixed_get_token(unsigned long size){
  return fixed_get_token_by(size, &fixed_allocator);
}

void fixed_free_token(unsigned long token){
  fixed_free_token_by(fixed_find_by_token(token, fixed_allocator));
}

unsigned long fixed_alloc(unsigned long token){
  return fixed_alloc_by(fixed_find_by_token(token, fixed_allocator));
}

void fixed_free(unsigned long token, unsigned long addr){
  fixed_free_by(addr, fixed_find_by_token(token, fixed_allocator), 0);
}

VariedAllocator varied_allocator = 0;
unsigned long varied_token = 0;

VariedAllocator newVariedAllocator(unsigned long addr,
    FixedAllocator fixed, VariedAllocator next){
  VariedAllocator new = (VariedAllocator)addr;
  new->fixed = fixed;
  new->next = next;
  return new;
}

VariedAllocator varied_find_by_token(unsigned long token, VariedAllocator varied){
  VariedAllocator aloctor = varied;
  while(aloctor)
    if(aloctor == (VariedAllocator)token) break;
    else aloctor = aloctor->next;
  return aloctor;
}

void varied_show(VariedAllocator varied, unsigned limit){
  if(!varied) return;
  printfmt("varied 0x%x ", varied);
  FixedAllocator fixed = varied->fixed;
  while(fixed){
    fixed_show(fixed, 3);
    fixed = fixed->next;
  }
}

unsigned long varied_get_token(){
  if(!varied_token) varied_token = fixed_get_token(sizeof(VariedAllocatorStr));
  varied_allocator = newVariedAllocator(fixed_alloc(varied_token), 0, varied_allocator);
  varied_log("varied token %x", varied_allocator);
  varied_log("");
  return (unsigned long)varied_allocator;
}

void varied_free_token(unsigned long token){
  VariedAllocator var = varied_find_by_token(token, varied_allocator);
  if(!var) return;
  FixedAllocator fixed = var->fixed;
  while(fixed){
    if(fixed_free_token_by(fixed)) return;
    fixed = fixed->next;
  }
  free_page((unsigned long)(var->fixed));
  varied_log("varied free token %x", token);
  varied_log("");
}

unsigned long varied_alloc(unsigned long token, unsigned long size){
  VariedAllocator var = varied_find_by_token(token, varied_allocator);
  unsigned long addr = fixed_alloc_by((FixedAllocator)fixed_get_token_by(
        pow2roundup(size), var ? &(var->fixed) : 0));
  varied_log("varied alloc %d bytes @0x%x", size, addr);
  varied_log("");
  return addr;
}

void varied_free(unsigned long token, unsigned long addr){
  VariedAllocator var = varied_find_by_token(token, varied_allocator);
  if(!var) return;
  FixedAllocator fixed = var->fixed;
  while(fixed){
    if(fixed_free_by(addr, fixed, 1)){
      varied_log("varied free 0x%x", addr);
      return;
    }
    fixed = fixed->next;
  }
}

unsigned long ktoken = 0;
unsigned long kmalloc(unsigned long size){
  if(!ktoken) ktoken = varied_get_token();
  return varied_alloc(ktoken, size);
}

void kfree(unsigned long addr){
  varied_free(ktoken, addr);
}
