#include "buddy.h"
#include "meta_macro.h"
#include "string_util.h"
#include "task.h"
#include "uart.h"

static struct buddy_page_node_struct * buddy_table_list[BUDDY_TABLE_LIST_LENGTH];
static struct buddy_page_node_struct * buddy_used_table_list[BUDDY_TABLE_LIST_LENGTH];
static struct buddy_page_pa_node_struct buddy_node_page_list[BUDDY_PAGE_PA_NODE_STRUCT_LENGTH];

void buddy_init(void)
{
  /* Assume nobody has asked for page yet */
  /* buddy_node_page_list[0].pa will take STARTUP_PAGE_PA_BASE */
  buddy_node_page_list[0].va = MMU_PA_TO_VA(mmu_startup_page_allocate(0));

  /* clear used_bit_array, clears 8 byte at once, but data type is 32bit(4 byte) */
  memzero_8byte((uint64_t *)buddy_node_page_list[0].used_bit_array, (BUDDY_BIT_ARRAY_LENGTH / 2));

  for(int i = 1; i < BUDDY_PAGE_PA_NODE_STRUCT_LENGTH; ++i)
  {
    buddy_node_page_list[i].va = 0x0;
  }

  /* init buddy_table_list and buddy_used_table_list as empty linked list */
  for(int i = 0; i < BUDDY_TABLE_LIST_LENGTH; ++i)
  {
    buddy_table_list[i] = 0x0;
    buddy_used_table_list[i] = 0x0;
  }


  /* buddy_page_node_struct_page = STARTUP_PAGE_PA_BASE */
  /* struct buddy_page_node_struct * is 8byte, 4KB = 512 * 8byte */
  buddy_new_page_node(0, MMU_PA_TO_VA((uint64_t *)(STARTUP_PAGE_PA_BASE + PAGE_4K)));
  for(unsigned i = 1; i < BUDDY_TABLE_LIST_LENGTH; ++i)
  {
    buddy_new_page_node(i, MMU_PA_TO_VA(STARTUP_PAGE_PA_BASE + (uint64_t)(PAGE_4K << i)));
  }
  uart_puts(ANSI_MAGENTA"[Buddy system]"ANSI_RESET" Startup page table transfer complete\n");
  uart_puts(ANSI_MAGENTA"[Buddy system]"ANSI_RESET" Init complete\n");
  return;
}

uint64_t * buddy_allocate(unsigned block_size, int zero, int to_pa)
{
  struct buddy_page_node_struct * ret_node;
  uint64_t * ret_ptr;

  TASK_GUARD();
  /* Check if list contains availible block */
  if(buddy_table_list[block_size] == 0)
  {
    if(buddy_split(block_size + 1) == 0)
    {
      /* Failed to split memory */
      uart_puts_blocking(ANSI_MAGENTA"[Buddy system]"ANSI_RESET" Failed to split memory");
      while(1);
      return 0x0;
    }
  }

  /* return the first availible block */
  ret_node = buddy_pop_page_node(&(buddy_table_list[block_size]), 0x0, buddy_table_list[block_size]);
  ret_ptr = ret_node -> va;

  /* put the current node into buddy_used_table_list */
  buddy_insert_node(&(buddy_used_table_list[block_size]), ret_node);

  if(zero)
  {
    memzero_8byte(ret_ptr, (PAGE_4K << block_size) / 8);
  }

#ifdef DEBUG
  char string_buff[0x20];

  uart_puts(ANSI_MAGENTA"[Buddy system]"ANSI_RESET" Allocate[Size:");
  string_longlong_to_char(string_buff, block_size);
  uart_puts(string_buff);
  uart_puts("]: ");
  string_ulonglong_to_hex_char(string_buff, (unsigned long long)ret_ptr);
  uart_puts(string_buff);
  uart_putc('\n');
#endif

  if(to_pa)
  {
    ret_ptr = MMU_VA_TO_PA(ret_ptr);
  }
  TASK_UNGUARD();
  return ret_ptr;
}

void buddy_free(uint64_t * va)
{
  TASK_GUARD();
  struct buddy_page_node_struct * node_to_free;

  /* traverse buddy_used_table_list to find the node */
  for(unsigned block_size_idx = 0; block_size_idx < BUDDY_TABLE_LIST_LENGTH; ++block_size_idx)
  {
    struct buddy_page_node_struct * cur_node = buddy_used_table_list[block_size_idx];
    struct buddy_page_node_struct * prev_node = buddy_used_table_list[block_size_idx];
    while(cur_node != 0 && cur_node -> va != va)
    {
      prev_node = cur_node;
      cur_node = cur_node -> next_ptr;
    }
    if(cur_node == 0)
    {
      continue;
    }

    node_to_free = cur_node;

    /* Remove node from buddy_used_table_list */
    buddy_pop_page_node(&(buddy_used_table_list[block_size_idx]), prev_node, node_to_free);

    /* Move the node to buddy_table_list */
    buddy_insert_node(&(buddy_table_list[block_size_idx]), node_to_free);

#ifdef DEBUG
    char string_buff[0x20];

    /* Print free complete messages */
    uart_puts(ANSI_MAGENTA"[Buddy system]"ANSI_RESET" Free[Size:");
    string_longlong_to_char(string_buff, block_size_idx);
    uart_puts(string_buff);
    uart_puts("]: ");
    string_ulonglong_to_hex_char(string_buff, (unsigned long long)va);
    uart_puts(string_buff);
    uart_putc('\n');
#endif

    /* Perform merge */
    buddy_merge(block_size_idx);
    TASK_UNGUARD();
    return;
  }

  char string_buffer[0x20];
  uart_puts("[Buddy system] Free failed: ");
  string_ulonglong_to_hex_char(string_buffer, (unsigned long long)va);
  uart_puts(string_buffer);
  uart_puts(" Entering busy while loop\n");
  while(1);
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
      /* 1. clear used_bit_array */
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
        return (struct buddy_page_node_struct *)((uint64_t)buddy_node_page_list[node_page_list_idx].va + sizeof(struct buddy_page_node_struct) * (bit_array_idx * 32 + (unsigned)bit_array_bit));
      }
    }
  }


  return 0x0;

}

void buddy_node_free(struct buddy_page_node_struct * node)
{
  for(unsigned node_page_list_idx = 0; node_page_list_idx < BUDDY_PAGE_PA_NODE_STRUCT_LENGTH; ++node_page_list_idx)
  {
    /* find the corresponding 4k page */
    if((uint64_t)buddy_node_page_list[node_page_list_idx].va == ((uint64_t)node & MMU_ADDR_MASK))
    {
      uint64_t offset = ((uint64_t)node - (uint64_t)buddy_node_page_list[node_page_list_idx].va) / sizeof(struct buddy_page_node_struct);
      unsigned bit_array_idx = (unsigned)(offset / 32);
      unsigned bit_array_bit = (unsigned)(offset - (bit_array_idx * 32));
      CLEAR_BIT(buddy_node_page_list[node_page_list_idx].used_bit_array[bit_array_idx], bit_array_bit);
      return;
    }
  }
}

void buddy_insert_node(struct buddy_page_node_struct ** list_head, struct buddy_page_node_struct * node)
{
  /* Insert node into empty list */
  if(*list_head == 0x0)
  {
    *list_head = node;
    node -> next_ptr = 0x0;
  }
  /* Find the correct place to insert */
  else
  {
    struct buddy_page_node_struct * prev_node = 0x0;
    struct buddy_page_node_struct * cur_node = *list_head;
    while(cur_node -> va < node -> va && cur_node -> next_ptr != 0)
    {
      prev_node = cur_node;
      cur_node = cur_node -> next_ptr;
    }
    /* list contains single node */
    if(prev_node == 0x0)
    {
      if(cur_node -> va < node -> va)
      {
        /* insert at end */
        node -> next_ptr = 0;
        cur_node -> next_ptr = node;
      }
      else
      {
        /* insert at start */
        node -> next_ptr = cur_node;
        *list_head = node;
      }
    }
    else
    {
      prev_node -> next_ptr = node;
      node -> next_ptr = cur_node;
    }
  }
}

/* insert node into buddy_table */
void buddy_new_page_node(unsigned buddy_table_list_block_size, uint64_t * va)
{
  struct buddy_page_node_struct * new_node = buddy_node_allocate();

  new_node -> va = va;
  buddy_insert_node(&(buddy_table_list[buddy_table_list_block_size]), new_node);
}

struct buddy_page_node_struct * buddy_pop_page_node(struct buddy_page_node_struct ** list_head, struct buddy_page_node_struct * prev_ptr, struct buddy_page_node_struct * cur_ptr)
{
  if(*list_head == cur_ptr)
  {
    *list_head = cur_ptr -> next_ptr;
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
  block_to_be_split_ptr = buddy_pop_page_node(&(buddy_table_list[block_size]), 0x0, buddy_table_list[block_size]);

  /* Insert the two splited node into smaller size linked list */
  buddy_new_page_node(block_size - 1, block_to_be_split_ptr -> va);
  buddy_new_page_node(block_size - 1, (uint64_t *)((uint64_t)(block_to_be_split_ptr -> va) + (PAGE_4K << (block_size - 1))));

#ifdef DEBUG
  char string_buff[0x20];

  uart_puts(ANSI_MAGENTA"[Buddy system]"ANSI_RESET" splitted. From[");
  string_longlong_to_char(string_buff, block_size);
  uart_puts(string_buff);
  uart_puts("]: ");
  string_ulonglong_to_hex_char(string_buff, (uint64_t)(block_to_be_split_ptr -> va));
  uart_puts(string_buff);
  uart_puts(" To[");
  string_longlong_to_char(string_buff, block_size - 1);
  uart_puts(string_buff);
  uart_puts("]: ");
  string_ulonglong_to_hex_char(string_buff, (uint64_t)(block_to_be_split_ptr -> va));
  uart_puts(string_buff);
  uart_puts(", ");
  string_ulonglong_to_hex_char(string_buff, (uint64_t)(block_to_be_split_ptr -> va) + (PAGE_4K << (block_size - 1)));
  uart_puts(string_buff);
  uart_putc('\n');
#endif

  /* Free the node */
  buddy_node_free(block_to_be_split_ptr);

  return 1;
}

int buddy_merge(unsigned block_size)
{
  /* Impossible to merge largest order block */
  if(block_size >= BUDDY_TABLE_LIST_LENGTH - 1)
  {
    return 0x0;
  }
  /* current block is empty, impossible to merge */
  /* need at least two block to perform merge */
  if(buddy_table_list[block_size] == 0x0 || buddy_table_list[block_size] -> next_ptr == 0x0)
  {
    return 0x0;
  }

  /* Merge current order blocks */
  struct buddy_page_node_struct * prev_prev_node = 0x0;
  struct buddy_page_node_struct * prev_node = buddy_table_list[block_size];
  struct buddy_page_node_struct * cur_node = buddy_table_list[block_size] -> next_ptr;
  while(cur_node != 0)
  {
    /* merge if buddy exist */
    /* The page frame number(PFN) and the block size order can be used to find the
     * page frame's buddy. For example, if a memory block's PFN is 8 and the block
     * size is 16KB (order = 2) you can find its buddy by 0b1000 xor 0b100 = 0b1100 = 12.
     */
    if((MMU_VA_TO_PFN(prev_node -> va) ^ (1uLL << block_size)) == MMU_VA_TO_PFN(cur_node -> va))
    {
      struct buddy_page_node_struct * new_node = buddy_node_allocate();

      buddy_pop_page_node(&(buddy_table_list[block_size]), prev_node, cur_node);
      buddy_pop_page_node(&(buddy_table_list[block_size]), prev_prev_node, prev_node);

      new_node -> va = prev_node -> va;
      buddy_insert_node(&(buddy_table_list[block_size + 1]), new_node);

#ifdef DEBUG
      char string_buff[0x20];

      uart_puts(ANSI_MAGENTA"[Buddy system]"ANSI_RESET" merged. From[");
      string_longlong_to_char(string_buff, block_size);
      uart_puts(string_buff);
      uart_puts("]: ");
      string_ulonglong_to_hex_char(string_buff, (uint64_t)(prev_node -> va));
      uart_puts(string_buff);
      uart_puts(", ");
      string_ulonglong_to_hex_char(string_buff, (uint64_t)(cur_node -> va));
      uart_puts(string_buff);
      uart_puts(" To[");
      string_longlong_to_char(string_buff, block_size + 1);
      uart_puts(string_buff);
      uart_puts("]: ");
      string_ulonglong_to_hex_char(string_buff, (uint64_t)(prev_node -> va));
      uart_puts(string_buff);
      uart_putc('\n');
#endif

      struct buddy_page_node_struct * cur_node_next_ptr = cur_node -> next_ptr;

      /* Free cur_node and prev_node */
      buddy_node_free(cur_node);
      buddy_node_free(prev_node);

      if(cur_node_next_ptr == 0)
      {
        break;
      }
    }
    /* advance pointer */
    prev_prev_node = prev_node;
    prev_node = cur_node;
    cur_node = cur_node -> next_ptr;

    /* list has less than two nodes */
    if(buddy_table_list[block_size] == 0x0 || buddy_table_list[block_size] -> next_ptr == 0x0)
    {
      break;
    }
  }


  /* Attempt to merge higher order blocks */
  buddy_merge(block_size + 1);

  return 0x0;
}

