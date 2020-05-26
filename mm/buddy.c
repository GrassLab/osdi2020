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
    buddy_info.bitmap[start / 8] |= 1 << (start % 8);
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
  for (i = 0; i < BUDDY_BLOCK_NUM; ++i)
    {
      if (buddy_info.bitmap[i / 8] & (1 << (i % 8)))
	{
	  uart_send ('1');
	}
      else
	{
	  uart_send ('_');
	}
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
    buddy_bitmap_set (target, order);
  buddy_status ();
  return target;
}
