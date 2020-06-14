#include "meta_macro.h"
#include "slab.h"
#include "buddy.h"
#include "uart.h"
#include "string_util.h"
#include "task.h"

static struct slab_struct * slab_list;
static struct malloc_struct * malloc_list;

uint64_t slab_regist(unsigned bytes)
{
  uint64_t token = (uint64_t)slab_find_or_create_slab(bytes);

#ifdef DEBUG
  char string_buff[0x20];

  uart_puts(ANSI_YELLOW"[Slab]"ANSI_RESET" Regist size: ");
  string_longlong_to_char(string_buff, bytes);
  uart_puts(string_buff);
  uart_puts(" Token: ");
  string_ulonglong_to_hex_char(string_buff, token);
  uart_puts(string_buff);
  uart_putc('\n');
#endif

  return token;
}

uint64_t * slab_allocate(uint64_t token)
{
  TASK_GUARD();
  for(unsigned bit_array_idx = 0; bit_array_idx < SLAB_USED_BIT_ARRAY_SIZE; ++bit_array_idx)
  {
    uint64_t * ret_ptr;

    /* see buddy_node_allocate for info */
    int bit_array_bit = __builtin_ffsl((int32_t)~(((struct slab_struct *)token) -> used_bit_array[bit_array_idx]));
    /* convert from 1 based to 0 based */
    --bit_array_bit;
    SET_BIT(((struct slab_struct *)token) -> used_bit_array[bit_array_idx], bit_array_bit);

    /* Change pointer to uint64_t to prevent offset multiplier */
    ret_ptr = (uint64_t *)((uint64_t)(((struct slab_struct *)token) -> va) + ((struct slab_struct *)token) -> object_size * (bit_array_idx * 32 + (unsigned)bit_array_bit));

#ifdef DEBUG
    char string_buff[0x20];

    uart_puts(ANSI_YELLOW"[Slab]"ANSI_RESET" Allocate size: ");
    string_longlong_to_char(string_buff, (long)(((struct slab_struct *)token) -> object_size));
    uart_puts(string_buff);
    uart_puts(" VA: ");
    string_ulonglong_to_hex_char(string_buff, (uint64_t)ret_ptr);
    uart_puts(string_buff);
    uart_putc('\n');
#endif

    TASK_UNGUARD();
    return ret_ptr;
  }
  /* failed to allocate space */
  uart_puts_blocking(ANSI_YELLOW"[Slab]"ANSI_RESET" Failed to allocate space\n");
  while(1);
  return 0x0;
}

void slab_free(uint64_t token, uint64_t * va)
{
  TASK_GUARD();
  uint64_t offset = ((uint64_t)va - (uint64_t)(((struct slab_struct *)token) -> va)) / (((struct slab_struct *)token) -> object_size);
  unsigned bit_array_idx = (unsigned)(offset / 32);
  unsigned bit_array_bit = (unsigned)(offset - (bit_array_idx * 32));

  CLEAR_BIT(((struct slab_struct *)token) -> used_bit_array[bit_array_idx], bit_array_bit);

#ifdef DEBUG
  char string_buff[0x20];

  uart_puts(ANSI_YELLOW"[Slab]"ANSI_RESET" Free size: ");
  string_longlong_to_char(string_buff, (long)(((struct slab_struct *)token) -> object_size));
  uart_puts(string_buff);
  uart_puts(" VA: ");
  string_ulonglong_to_hex_char(string_buff, (uint64_t)va);
  uart_puts(string_buff);
  uart_putc('\n');
#endif
  TASK_UNGUARD();
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

uint64_t * slab_malloc(unsigned bytes)
{
  TASK_GUARD();
  uint64_t * ret_ptr;
  /*Use buddy system for higher volume
   *kmalloc-2k
   *kmalloc-1k
   *kmalloc-512
   *kmalloc-256
   *kmalloc-192 (ignored)
   *kmalloc-128
   *kmalloc-96 (ignored)
   *kmalloc-64
   *kmalloc-32
   *kmalloc-16
   *kmalloc-8
   */
  uint32_t lower_bytes = (uint32_t)bytes;
  int leading_zeroes = __builtin_clz(lower_bytes);

  struct malloc_struct * new_node = (struct malloc_struct *)slab_allocate(slab_regist(sizeof(struct malloc_struct)));

  if(malloc_list == 0)
  {
    malloc_list = new_node;
    new_node -> next_ptr = 0;
  }
  else /* prepend to list */
  {
    new_node -> next_ptr = malloc_list;
    malloc_list = new_node;
  }

  if(bytes >= PAGE_4K)
  {
    int block_size = (19 - leading_zeroes);
    /* 0x1000 leading_zeroes = 19 */
    /* 0x2000 leading_zeroes = 18 */

    CLEAR_BIT(lower_bytes, block_size + 12);
    if(lower_bytes == 0)
    {
      /* lower_bytes is power of 2 */
      ret_ptr = buddy_allocate((unsigned)block_size, BUDDY_ALLOCATE_NO_MEM_ZERO, BUDDY_ALLOCATE_TO_VA);
    }
    else
    {
      /* allocate one order larger */
      ret_ptr = buddy_allocate((unsigned)block_size + 1, BUDDY_ALLOCATE_NO_MEM_ZERO, BUDDY_ALLOCATE_TO_VA);
    }
    new_node -> token = 0;
  }
  else
  {
    /* 0x8 leading_zeroes = 28 */
    int fixed_size = (28 - leading_zeroes);
    uint64_t token;

    CLEAR_BIT(lower_bytes, fixed_size + 3);

    if(lower_bytes == 0)
    {
      /* lower_bytes is power of 2 */
      token = slab_regist(1u << (fixed_size + 3));
      ret_ptr = slab_allocate(token);
    }
    else
    {
      /* allocate one order larger */
      token = slab_regist(1u << (fixed_size + 3 + 1));
      ret_ptr = slab_allocate(token);
    }
    new_node -> token = token;
  }

  new_node -> va = (uint64_t *)ret_ptr;

#ifdef DEBUG
  char string_buff[0x20];

  uart_puts(ANSI_BLUE"[Malloc]"ANSI_RESET" Allocate Size: ");
  string_ulonglong_to_hex_char(string_buff, bytes);
  uart_puts(string_buff);
  uart_puts(" Token: ");
  string_ulonglong_to_hex_char(string_buff, new_node -> token);
  uart_puts(string_buff);
  uart_puts(" VA: ");
  string_ulonglong_to_hex_char(string_buff, (uint64_t)ret_ptr);
  uart_puts(string_buff);
  uart_putc('\n');
#endif

  TASK_UNGUARD();
  return ret_ptr;
}

void slab_malloc_free(uint64_t * va)
{
  TASK_GUARD();
  struct malloc_struct * cur_node = malloc_list;
  struct malloc_struct * prev_node = 0x0;

  while(cur_node != 0x0)
  {
    if(cur_node -> va == va)
    {
      if(prev_node == 0x0)
      {
        malloc_list = cur_node -> next_ptr;
      }
      else
      {
        prev_node -> next_ptr = cur_node -> next_ptr;
      }

      if(cur_node -> token == 0)
      {
        buddy_free(cur_node -> va);
      }
      else
      {
        slab_free(cur_node -> token, cur_node -> va);
      }
      slab_free(slab_regist(sizeof(struct malloc_struct)), (uint64_t *)cur_node);

#ifdef DEBUG
      char string_buff[0x20];

      uart_puts(ANSI_BLUE"[Malloc]"ANSI_RESET" Free VA: ");
      string_ulonglong_to_hex_char(string_buff, (uint64_t)va);
      uart_puts(string_buff);
      uart_putc('\n');
#endif

      return;
    }
    prev_node = cur_node;
    cur_node = cur_node -> next_ptr;
  }
  TASK_UNGUARD();
  return;
}

