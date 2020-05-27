#include "buddy.h"
#include "meta_macro.h"
#include "string_util.h"
#include "task.h"
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
    buddy_insert_page_node(i, MMU_PA_TO_VA(STARTUP_PAGE_PA_BASE + (uint64_t)(PAGE_4K << i)));
  }
  uart_puts(ANSI_MAGENTA"[Buddy system]"ANSI_RESET" Startup page table transfer complete\n");
  uart_puts(ANSI_MAGENTA"[Buddy system]"ANSI_RESET" Init complete\n");
  return;
}

uint64_t * buddy_allocate(unsigned block_size, int zero, int to_pa)
{
  char string_buff[0x20];
  uint64_t * ret_ptr;

  task_guard_section();
  /* Check if list contains availible block */
  if(buddy_table_list[block_size] == 0)
  {
    if(buddy_split(block_size + 1) == 0)
    {
      /* Failed to split memory */
      return 0x0;
    }
  }

  /* return the first availible block */
  ret_ptr = buddy_pop_page_node(block_size, 0x0, buddy_table_list[block_size]) -> va;

  task_unguard_section();
  if(zero)
  {
    memzero_8byte(ret_ptr, (PAGE_4K << block_size) / 8);
  }

  uart_puts(ANSI_MAGENTA"[Buddy system]"ANSI_RESET" Allocate[Size:");
  string_longlong_to_char(string_buff, block_size);
  uart_puts(string_buff);
  uart_puts("]: ");
  string_ulonglong_to_hex_char(string_buff, (unsigned long long)ret_ptr);
  uart_puts(string_buff);
  uart_putc('\n');

  if(to_pa)
  {
    ret_ptr = MMU_VA_TO_PA(ret_ptr);
  }
  return ret_ptr;
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
      /* Fail to find space for current page */
      /* Get another 4k page from buddy system itself */
      /* Setup node_page_list_idx, bit_array_idx, bit_array_bit */
      //buddy_node_page_list[node_page_list_idx].va = MMU_PA_TO_VA(mmu_startup_page_allocate(0));
      /* TODO */
      uart_puts_blocking("Unimplemented error on buddy_node_allocate\n");
      while(1);
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


  return 0x0;

}

/* insert node into buddy_table */
void buddy_insert_page_node(unsigned buddy_table_list_block_size, uint64_t * va)
{
  struct buddy_page_node_struct * new_node = buddy_node_allocate();

  new_node -> va = va;

  /* Insert new node into empty list */
  if(buddy_table_list[buddy_table_list_block_size] == 0x0)
  {
    buddy_table_list[buddy_table_list_block_size] = new_node;
    new_node -> next_ptr = 0x0;
  }
  /* Find the correct place to insert */
  else
  {
    struct buddy_page_node_struct * prev_node;
    struct buddy_page_node_struct * cur_node = buddy_table_list[buddy_table_list_block_size];
    while(cur_node -> va < va && cur_node -> next_ptr != 0)
    {
      prev_node = cur_node;
      cur_node = cur_node -> next_ptr;
    }
    if(cur_node -> next_ptr == 0x0)
    {
      /* insert at end of list */
      cur_node -> next_ptr = new_node;
      new_node -> next_ptr = 0x0;
    }
    else
    {
      prev_node -> next_ptr = new_node;
      new_node -> next_ptr = cur_node;
    }
  }
}

struct buddy_page_node_struct * buddy_pop_page_node(unsigned buddy_table_list_block_size, struct buddy_page_node_struct * prev_ptr, struct buddy_page_node_struct * cur_ptr)
{
  if(buddy_table_list[buddy_table_list_block_size] == cur_ptr)
  {
    buddy_table_list[buddy_table_list_block_size] = cur_ptr -> next_ptr;
  }
  else if(cur_ptr -> next_ptr == 0x0)
  {
    prev_ptr -> next_ptr = 0x0;
  }
  else
  {
    prev_ptr -> next_ptr = cur_ptr -> next_ptr;
  }
  return cur_ptr;
}

/* block size is the size you want to split */
int buddy_split(unsigned block_size)
{
  char string_buff[0x20];
  if(block_size == 4)
  {
    block_size = block_size + 1;
    block_size = block_size - 1;
  }

  struct buddy_page_node_struct * block_to_be_split_ptr;
  if(block_size >= BUDDY_TABLE_LIST_LENGTH || block_size == 0)
  {
    return 0x0;
  }
  /* current block is empty, attempt to split larger block */
  if(buddy_table_list[block_size] == 0x0)
  {
    if(buddy_split(block_size + 1) == 0)
    {
      /* Fail to split block */
      uart_puts_blocking(ANSI_MAGENTA"[Buddy system]"ANSI_RESET" Failed to allocate memory by splitting.\n");
      uart_puts_blocking("Entering busy while loop");
      while(1);
      return 0x0;
    }
  }
  /* Remove the first block from the linked list */
  block_to_be_split_ptr = buddy_pop_page_node(block_size, 0x0, buddy_table_list[block_size]);

  /* Insert the two splited node into smaller size linked list */
  buddy_insert_page_node(block_size - 1, block_to_be_split_ptr -> va);
  buddy_insert_page_node(block_size - 1, (uint64_t *)((uint64_t)(block_to_be_split_ptr -> va) + (PAGE_4K << (block_size - 1))));

  /* TODO: Free the node */

  uart_puts(ANSI_MAGENTA"[Buddy system]"ANSI_RESET" Size ");
  string_longlong_to_char(string_buff, block_size);
  uart_puts(string_buff);
  uart_puts(" splitted\n");
  return 1;
}

