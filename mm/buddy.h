#ifndef BUDDY
#define BUDDY

#include <stddef.h>
#define BUDDY_ORDER_MAX 8
#define BUDDY_BLOCK_MIN PAGE_SIZE
#define BUDDY_BLOCK_NUM (1 << BUDDY_ORDER_MAX)
#define BUDDY_START 0xffffb0dd90000000

struct buddy_struct
{
  void *start;
  void *end;
  size_t size;
  char bitmap[BUDDY_BLOCK_NUM];
  struct buddy_chunk
  {
    // TODO: double link list
    struct buddy_chunk *next;
  } *bins[BUDDY_ORDER_MAX + 1];
} buddy_info;

void *buddy_malloc (size_t size);

#endif /* ifndef BUDDY */
