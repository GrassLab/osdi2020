#include <stddef.h>
#include <string.h>
#include <uart.h>
#include "tlb.h"
#include "buddy.h"

void
buddy_init ()
{
  buddy_info.size = BUDDY_BLOCK_MIN * BUDDY_BLOCK_NUM;
  buddy_info.start =
    page_alloc_virt (KPGD, BUDDY_START, buddy_info.size / PAGE_SIZE,
		     pd_encode_ram (0));
  if (buddy_info.start == NULL)
    {
      printf ("%s\r\n", "TODO: buddy_init error");
      while (1);
    }
  buddy_info.end = buddy_info.start + buddy_info.size;
  bzero (buddy_info.start, sizeof (struct buddy_chunk));
  buddy_info.bins[BUDDY_ORDER_MAX] = buddy_info.start;
}

void *
buddy_find_block (size_t order)
{
  struct buddy_chunk *target;
  struct buddy_chunk *remain;
  if (order > BUDDY_ORDER_MAX)
    return NULL;
  if (buddy_info.bins[order] != NULL)
    {
      target = buddy_info.bins[order];
      buddy_info.bins[order] = target->next;
      return target;
    }
  target = buddy_find_block (order + 1);
  if (target == NULL)
    return NULL;
  // split and insert remain block into bins
  remain = (void *) target + BUDDY_BLOCK_MIN * (1 << order);
  remain->next = buddy_info.bins[order];
  buddy_info.bins[order] = remain;
  return target;
}

// TODO: faster
void
buddy_bitmap_set (void *addr, size_t order)
{
  size_t start, end;
  start = (size_t) (addr - buddy_info.start) / BUDDY_BLOCK_MIN;
  end = start + (1 << order);
  for (; start < end; ++start)
    buddy_info.bitmap[start] = order + 1;
}

size_t
buddy_bitmap_reset (void *addr)
{
  size_t start, end, order;
  start = (size_t) (addr - buddy_info.start) / BUDDY_BLOCK_MIN;
  order = buddy_info.bitmap[start] - 1;
  end = start + (1 << order);
  for (; start < end; ++start)
    buddy_info.bitmap[start] = 0;
  return order;
}

void
buddy_status ()
{
  size_t i;
  struct buddy_chunk *p;

  printf ("buddy b0dd9: %p-%p\n", buddy_info.start, buddy_info.end);
  for (i = 0; i <= BUDDY_ORDER_MAX; ++i)
    {
      printf ("%d: ", (int) i);
      p = buddy_info.bins[i];
      for (; p != NULL; p = p->next)
	{
	  printf ("%p -> ", p);
	}
      uart_puts ("\r\n");
    }
  printf ("%s\r\n", "buddy bitmap:");
  char c;
  for (i = 0; i < BUDDY_BLOCK_NUM; ++i)
    {
      c = buddy_info.bitmap[i];
      if (c == 0)
	c = '_';
      else
	c += '0' - 1;
      uart_send (c);
    }
  uart_puts ("\r\n");
}

void *
buddy_malloc (size_t size)
{
  size_t order;
  void *target;

  if (buddy_info.start == NULL)
    buddy_init ();
  // calculate order
  for (order = 0; size > BUDDY_BLOCK_MIN * (1 << order); ++order);

  target = buddy_find_block (order);
  if (target != NULL)
    {
      buddy_bitmap_set (target, order);
      bzero (target, BUDDY_BLOCK_MIN * (1 << order));
    }
  buddy_status ();
  return target;
}

int
buddy_is_freed (size_t ind, size_t order)
{
  size_t i;
  for (i = 0; i < (1 << order); ++i)
    if (buddy_info.bitmap[ind + i])
      return 0;
  return 1;
}

void
buddy_merge (size_t buddy_ind, size_t buddy_order)
{
  size_t bro_ind;
  size_t merged_ind;
  struct buddy_chunk *bro;
  struct buddy_chunk *p;
  if (buddy_order >= BUDDY_ORDER_MAX)
    {
      printf ("buddy_merge error: order too large");
      while (1);
    }
  merged_ind = buddy_ind;
  bro_ind = buddy_ind ^ (1 << buddy_order);
  if (buddy_is_freed (bro_ind, buddy_order))
    {
      bro = buddy_info.start + BUDDY_BLOCK_MIN * bro_ind;
      // unlink bro
      // TODO: use list head
      p = buddy_info.bins[buddy_order];
      if (p == bro)
	buddy_info.bins[buddy_order] = p->next;
      else
	{
	  for (; p != NULL; p = p->next)
	    {
	      if (p->next == bro)
		{
		  p->next = bro->next;
		  break;
		}
	    }
	}
      merged_ind = (buddy_ind < bro_ind) ? buddy_ind : bro_ind;
      if (buddy_order < BUDDY_ORDER_MAX - 1)
	{
	  // try to find next bro
	  buddy_merge (merged_ind, buddy_order + 1);
	}
      else
	{
	  // insert
	  // TODO: use list head
	  p = buddy_info.start + BUDDY_BLOCK_MIN * merged_ind;
	  p->next = buddy_info.bins[buddy_order + 1];
	  buddy_info.bins[buddy_order + 1] = p;
	}
    }
  else
    {
      // insert
      p = buddy_info.start + BUDDY_BLOCK_MIN * merged_ind;
      p->next = buddy_info.bins[buddy_order];
      buddy_info.bins[buddy_order] = p;
    }
}

void
buddy_free (void *addr)
{
  size_t ind, order;
  if (addr < buddy_info.start || addr >= buddy_info.end)
    return;
  order = buddy_bitmap_reset (addr);
  ind = (size_t) (addr - buddy_info.start) / BUDDY_BLOCK_MIN;
  buddy_merge (ind, order);
  buddy_status ();
}

void *
buddy_realloc (void *addr, size_t size)
{
  size_t ind, order;
  size_t old_size;
  void *new;
  ind = (size_t) (addr - buddy_info.start) / BUDDY_BLOCK_MIN;
  order = buddy_info.bitmap[ind] - 1;
  old_size = BUDDY_BLOCK_MIN * (1 << order);
  if (size <= old_size)
    return addr;
  new = buddy_malloc (size);
  if (new == 0)
    {
      printf ("%s\r\n", "buddy_realloc fail");
      while (1);
    }
  memcpy (new, addr, old_size);
  buddy_free (addr);
  return new;
}
