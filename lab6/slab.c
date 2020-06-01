#include "meta_macro.h"
#include "slab.h"
#include "buddy.h"
#include "uart.h"
#include "string_util.h"

static struct slab_struct * slab_list;

uint64_t slab_regist(unsigned bytes)
{
  char string_buff[0x20];
  uint64_t token = (uint64_t)slab_find_or_create_slab(bytes);

  uart_puts(ANSI_YELLOW"[Slab]"ANSI_RESET" Regist size: ");
  string_longlong_to_char(string_buff, bytes);
  uart_puts(string_buff);
  uart_puts(" Token: ");
  string_ulonglong_to_hex_char(string_buff, token);
  uart_puts(string_buff);
  uart_putc('\n');

  return token;
}

uint64_t * slab_allocate(uint64_t token)
{
  for(unsigned bit_array_idx = 0; bit_array_idx < SLAB_USED_BIT_ARRAY_SIZE; ++bit_array_idx)
  {
    char string_buff[0x20];
    uint64_t * ret_ptr;

    /* see buddy_node_allocate for info */
    int bit_array_bit = __builtin_ffsl((int32_t)~(((struct slab_struct *)token) -> used_bit_array[bit_array_idx]));
    /* convert from 1 based to 0 based */
    --bit_array_bit;
    SET_BIT(((struct slab_struct *)token) -> used_bit_array[bit_array_idx], bit_array_bit);

    /* Change pointer to uint64_t to prevent offset multiplier */
    ret_ptr = (uint64_t *)((uint64_t)(((struct slab_struct *)token) -> va) + ((struct slab_struct *)token) -> object_size * (bit_array_idx * 32 + (unsigned)bit_array_bit));

    uart_puts(ANSI_YELLOW"[Slab]"ANSI_RESET" Allocate size: ");
    string_longlong_to_char(string_buff, (long)(((struct slab_struct *)token) -> object_size));
    uart_puts(string_buff);
    uart_puts(" VA: ");
    string_ulonglong_to_hex_char(string_buff, (uint64_t)ret_ptr);
    uart_puts(string_buff);
    uart_putc('\n');

    return ret_ptr;
  }
  /* failed to allocate space */
  return 0x0;
}

void slab_free(uint64_t token, uint64_t * va)
{
  char string_buff[0x20];

  uint64_t offset = ((uint64_t)va - (uint64_t)(((struct slab_struct *)token) -> va)) / (((struct slab_struct *)token) -> object_size);
  unsigned bit_array_idx = (unsigned)(offset / 32);
  unsigned bit_array_bit = (unsigned)(offset - (bit_array_idx * 32));

  CLEAR_BIT(((struct slab_struct *)token) -> used_bit_array[bit_array_idx], bit_array_bit);

  uart_puts(ANSI_YELLOW"[Slab]"ANSI_RESET" Free size: ");
  string_longlong_to_char(string_buff, (long)(((struct slab_struct *)token) -> object_size));
  uart_puts(string_buff);
  uart_puts(" VA: ");
  string_ulonglong_to_hex_char(string_buff, (uint64_t)va);
  uart_puts(string_buff);
  uart_putc('\n');
  return;
}

struct slab_struct * slab_find_or_create_slab(unsigned bytes)
{
  /* slab_list is empty */
  if(slab_list == 0x0)
  {
    struct slab_struct * slab_ptr = slab_allocate_slab();
    slab_ptr -> object_size = bytes;
    slab_insert_slab(slab_ptr);
    return slab_ptr;
  }

  /* attempt to find slab */
  struct slab_struct * cur_node = slab_list;
  while(cur_node -> object_size < bytes && cur_node -> next_ptr != 0)
  {
    cur_node = cur_node -> next_ptr;
  }
  if(cur_node -> object_size == bytes)
  {
    return cur_node;
  }
  else
  {
    struct slab_struct * slab_ptr = slab_allocate_slab();
    slab_ptr -> object_size = bytes;
    slab_insert_slab(slab_ptr);
    return slab_ptr;
  }
}

struct slab_struct * slab_allocate_slab(void)
{
  static uint64_t * current_free_space_ptr;
  struct slab_struct * new_slab;

  /* Initialize */
  if(current_free_space_ptr == 0x0)
  {
    current_free_space_ptr = buddy_allocate(0, BUDDY_ALLOCATE_MEM_ZERO, BUDDY_ALLOCATE_TO_VA);
  }

  /* TODO: If current page is full */

  new_slab = (struct slab_struct *)current_free_space_ptr;
  new_slab -> va = buddy_allocate(0, BUDDY_ALLOCATE_NO_MEM_ZERO, BUDDY_ALLOCATE_TO_VA);

  current_free_space_ptr += sizeof(struct slab_struct);
  return new_slab;
}

void slab_insert_slab(struct slab_struct * node)
{
  /* insert at head if list is empty */
  if(slab_list == 0x0)
  {
    slab_list = node;
    return;
  }

  /* find the correct position */
  struct slab_struct * prev_node = 0x0;
  struct slab_struct * cur_node = slab_list;
  while(cur_node -> object_size < node -> object_size && cur_node -> next_ptr != 0)
  {
    prev_node = cur_node;
    cur_node = cur_node -> next_ptr;
  }
  if(cur_node -> next_ptr == 0x0)
  {
    /* insert at end of list */
    cur_node -> next_ptr = node;
    node -> next_ptr = 0x0;
  }
  else if(prev_node == 0x0)
  {
    /* insert at head */
    node -> next_ptr = slab_list;
    slab_list = node;
  }
  else
  {
    prev_node -> next_ptr = node;
    node -> next_ptr = cur_node;
  }
}

