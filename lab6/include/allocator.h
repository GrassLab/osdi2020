#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

/* buddy system */
#define MAX_ORDER 10

typedef struct cdr_tag {
  unsigned long val;
  struct cdr_tag *cdr;
} CdrStr, *Cdr;

struct free_area {
  Cdr                free_list;
  unsigned long      nr_free;
};

typedef struct zone_tag {
  /* free areas of differents sizes */
  struct free_area        free_area[MAX_ORDER];
} ZoneStr, *Zone;

extern Zone const buddy_zone;

void zone_init(Zone);
unsigned long zone_get_free_pages(Zone zone, int order);
void zone_free_pages(Zone zone, unsigned long addr);

void zone_show(Zone zone, unsigned long cnt);

#define addr2pgidx(addr) ((addr - ALOC_BEG) / PAGE_SIZE)

#define enable_buddy_log 1

#if enable_buddy_log
#define buddy_log(...) printfmt(__VA_ARGS__)
#define buddy_log_graph(zone) zone_show(zone, 3)
#else
#define buddy_log(...) ;
#define buddy_log_graph(zone) ;
#endif

/* fixed size allocator below */

// 536
typedef struct FixedBook_tag {
  unsigned long page_addr;
  unsigned free_nr;
  char table[512];
  struct fixedSizeBookTable_tag *next;
} FixedBookStr, *FixedBook;

typedef struct FixedAllocator_tag {
  /* free areas of differents sizes */
  unsigned long size;
  struct fallocator_tag *next;
  FixedBook book;
} FixedAllocatorStr, *FixedAllocator;

unsigned long fixed_sized_get_token(unsigned long size);
unsigned long fixed_sized_alloc(unsigned long token);
unsigned long fixed_sized_free(unsigned long addr);

#endif
