#include <string.h>
#include "varied.h"

static struct varied_struct varied_info;

int
varied_init ()
{
  size_t i;
  varied_info.node_token = token_register (sizeof (struct allocated_node));
  if (varied_info.node_token == 0)
    return 1;
  for (i = 0; i < VARIED_TOKEN_LEN; ++i)
    {
      varied_info.tokens[i] = token_register (VARIED_MIN * (i + 1));
      if (varied_info.tokens[i] == 0)
	return 1;
    }
  INIT_LIST_HEAD (&varied_info.chunk_head);
  return 0;
}

void *
varied_malloc (size_t size)
{
  size_t token_ind;
  struct allocated_node *new_node;
  void *addr;

  // try to init varied_info
  if (varied_info.tokens[0] == 0)
    if (varied_init ())
      return NULL;
  // new a node to record allocated chunk
  new_node = fixed_malloc (varied_info.node_token);
  if (new_node == NULL)
    return NULL;
  // select allocator
  if (size > VARIED_MAX)
    {
      addr = buddy_malloc (size);
      new_node->free = buddy_free;
    }
  else
    {
      token_ind = size / VARIED_MIN - 1;
      if (size % VARIED_MIN)
	++token_ind;
      addr = fixed_malloc (varied_info.tokens[token_ind]);
      new_node->free = fixed_free;
    }
  new_node->request_size = size;
  // Record node
  if (addr)
    {
      new_node->addr = addr;
      list_add (&new_node->list, &varied_info.chunk_head);
    }
  else
    {
      fixed_free (new_node);
    }
  return addr;
}

void
varied_free (void *addr)
{
  struct allocated_node *node;
  list_for_each_entry (node, &varied_info.chunk_head, list)
  {
    if (node->addr == addr)
      {
	// free target
	node->free (addr);
	// free node
	list_del (&node->list);
	fixed_free (node);
	return;
      }
  }
}

void *
varied_realloc (void *addr, size_t size)
{
  struct allocated_node *node;
  void *victim;
  list_for_each_entry (node, &varied_info.chunk_head, list)
  {
    if (node->addr == addr)
      {
	// TODO: it's possible that new chunk size == old chunk size
	// because we record unaligned request_size from user
	if (size > node->request_size)
	  {
	    victim = varied_malloc (size);
	    if (victim == NULL)
	      return NULL;
	    memcpy (victim, addr, node->request_size);
	    node->free (addr);
	    node->addr = victim;
	  }
	return node->addr;
      }
  }
  return NULL;
}
