#include "buddy.h"
#include "meta_macro.h"
#include "string_util.h"
#include "uart.h"

static struct buddy_page_node_struct * buddy_table_list[BUDDY_TABLE_LIST_LENGTH];
static struct buddy_page_pa_node_struct buddy_node_page_list[BUDDY_PAGE_PA_NODE_STRUCT_LENGTH];

void buddy_init(void)
{
  /* Assume nobody has asked for page yet */
  /* buddy_node_page_list[0].pa will take STARTUP_PAGE_PA_BASE */
  buddy_node_page_list[0].va = MMU_PA_TO_VA(mmu_startup_page_allocate(0));
  memzero_8byte(buddy_node_page_list[0].used_bit_array, 8);
  for(int i = 1; i < BUDDY_PAGE_PA_NODE_STRUCT_LENGTH; ++i)
  {
    buddy_node_page_list[i].va = 0x0;
  }

  /* init each buddy_table_list as empty linked list */
  for(int i = 0; i < BUDDY_TABLE_LIST_LENGTH; ++i)
  {
    buddy_table_list[i] = 0x0;
  }


  /* buddy_page_node_struct_page = STARTUP_PAGE_PA_BASE */
  /* struct buddy_page_node_struct * is 8byte, 4KB = 512 * 8byte */
  buddy_insert_page_node(0, (uint64_t *)(STARTUP_PAGE_PA_BASE + PAGE_4K));
  for(unsigned i = 1; i < BUDDY_TABLE_LIST_LENGTH; ++i)
  {
    buddy_insert_page_node(i, (uint64_t *)(STARTUP_PAGE_PA_BASE + PAGE_4K * (1uLL << i)));
  }
  uart_puts(ANSI_MAGENTA"[Buddy system]"ANSI_RESET" Startup page table transfer complete\n");
  uart_puts(ANSI_MAGENTA"[Buddy system]"ANSI_RESET" Init complete\n");
  return;
}

struct buddy_page_node_struct * buddy_node_allocate(void)
{
  unsigned node_page_list_idx;
  unsigned bit_array_idx;
  int bit_array_bit;

  for(node_page_list_idx = 0; node_page_list_idx < BUDDY_PAGE_PA_NODE_STRUCT_LENGTH; ++node_page_list_idx)
  {
    if(buddy_node_page_list[node_page_list_idx].va == 0)
    {
      continue;
    }
    for(bit_array_idx = 0; bit_array_idx < BUDDY_BIT_ARRAY_LENGTH; ++bit_array_idx)
    {
      bit_array_bit = __builtin_ffsl((int32_t)~(buddy_node_page_list[node_page_list_idx].used_bit_array[bit_array_idx]));
      if(bit_array_bit != 0)
      {
        /* for a 64bit * 8 array, unset bit means the space is not occupied, set if occupied */
        /* flip 0 to 1 and 1 to 0, then use ffs to get the LSB 1 position, we can get the first unused space, which is actually 0 */
        /* example
          uint64_t a = (uint64_t)-1LL;
          uint64_t b = 0LL;
          uint64_t c = 1LL;
          int la = __builtin_ffsll((int64_t)~a);
          int lb = __builtin_ffsll((int64_t)~b);
          int lc = __builtin_ffsll((int64_t)~c);
          printf("%d %d %d", la, lb, lc);
          // la, lb, lc = 0, 1, 2
         */
        /* convert from 1 based to 0 based */
        --bit_array_bit;
        SET_BIT(buddy_node_page_list[node_page_list_idx].used_bit_array[bit_array_idx], bit_array_bit);

        /* Change pointer to uint64_t to prevent offset multiplier */
        return (struct buddy_page_node_struct *)((uint64_t)buddy_node_page_list[node_page_list_idx].va + sizeof(struct buddy_page_node_struct) * (bit_array_idx * 64 + (unsigned)bit_array_bit));
      }
    }
  }

  /* Fail to find space for current page */
  /* TODO: Get 4k page from buddy system itself */
  /* Setup node_page_list_idx, bit_array_idx, bit_array_bit */

  return 0x0;

}

/* insert node into buddy_table */
void buddy_insert_page_node(unsigned buddy_table_list_block_size, uint64_t * va)
{
  struct buddy_page_node_struct * new_node = buddy_node_allocate();

  new_node -> va = va;

  /* Insert new node into list */
  if(buddy_table_list[buddy_table_list_block_size] == 0x0)
  {
    buddy_table_list[buddy_table_list_block_size] = new_node;
    new_node -> next_ptr = 0x0;
  }
  /* TODO: insert if list_head is not null */
}

