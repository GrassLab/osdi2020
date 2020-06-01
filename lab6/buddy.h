#include <stdint.h>
#include "mmu.h"
#ifndef __BUDDY_H__
#define __BUDDY_H__

#define BUDDY_ALLOCATE_TO_VA 0
#define BUDDY_ALLOCATE_TO_PA 1

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

/* 512 / 32bit = 16 */
/* choose 32 bit because __builtin_ffsll is bugged */
#define BUDDY_BIT_ARRAY_LENGTH 16
struct buddy_page_pa_node_struct
{
  /* struct buddy_page_node_struct * is 8byte, 4KB = 512 * 8byte */
  /* 512 / 32bit = 16 (BUDDY_BIT_ARRAY_LENGTH) */
  uint32_t used_bit_array[BUDDY_BIT_ARRAY_LENGTH];
  uint64_t * va;
};

/* struct buddy_page_pa_node_struct is 4 * 16 + 8 = 72 bytes */
/* 14 * (72 bytes) sizeof(struct buddy_page_pa_node_struct) = 1008 < 1024(1KB */
#define BUDDY_PAGE_PA_NODE_STRUCT_LENGTH 14

void buddy_init(void);
uint64_t * buddy_allocate(unsigned block_size, int zero, int to_pa);
void buddy_free(uint64_t * va);

struct buddy_page_node_struct * buddy_node_allocate(void);

void buddy_insert_node(struct buddy_page_node_struct ** list_head, struct buddy_page_node_struct * node);
void buddy_new_page_node(unsigned buddy_table_list_block_size, uint64_t * va);
struct buddy_page_node_struct * buddy_pop_page_node(struct buddy_page_node_struct ** list_head, struct buddy_page_node_struct * prev_ptr, struct buddy_page_node_struct * cur_ptr);
int buddy_split(unsigned block_size);
int buddy_merge(unsigned block_size);

#endif

