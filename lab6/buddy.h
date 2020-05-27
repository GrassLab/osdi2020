#include <stdint.h>
#include "mmu.h"
#ifndef __BUDDY_H__
#define __BUDDY_H__

/* Page linked list for buddy system, split 8MB to */
/* set to 4MB, 2MB, 1MB, 512KB, 256KB, 128KB, 64KB, 32KB, 16KB, 8KB, 4KB */
/* 11 size in total */
#define BUDDY_TABLE_LIST_LENGTH 11

/* 16 byte */
struct buddy_page_node_struct
{
  struct buddy_page_node_struct * next_ptr;
  uint64_t * va;
};

/* 14 * sizeof(struct buddy_page_pa_node_struct) = 1008 < 1024(1KB */
#define BUDDY_PAGE_PA_NODE_STRUCT_LENGTH 14
/* 512 / 64bit = 8 */
#define BUDDY_BIT_ARRAY_LENGTH 8
struct buddy_page_pa_node_struct
{
  /* struct buddy_page_node_struct * is 8byte, 4KB = 512 * 8byte */
  /* 512 / 64bit = 8 */
  uint64_t used_bit_array[BUDDY_BIT_ARRAY_LENGTH];
  uint64_t * va;
};

void buddy_init(void);
struct buddy_page_node_struct * buddy_node_allocate(void);
void buddy_insert_page_node(struct buddy_page_node_struct ** list_head, uint64_t * pa);

#endif

