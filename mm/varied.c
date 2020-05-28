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
  INIT_LIST_HEAD (&varied_info.buddy_head);
  INIT_LIST_HEAD (&varied_info.fixed_head);
  return 0;
}

void *
varied_malloc (size_t size)
{
  size_t token_ind;
  struct list_head *target_head;
  struct allocated_node *new_node;
  void *addr;
  if (varied_info.tokens[0] == 0)
    if (varied_init ())
      return NULL;
  if (size > VARIED_MAX)
    {
      addr = buddy_malloc (size);
      target_head = &varied_info.buddy_head;
    }
  else
    {
      token_ind = size / VARIED_MIN - 1;
      if (size % VARIED_MIN)
	++token_ind;
      addr = fixed_malloc (varied_info.tokens[token_ind]);
      target_head = &varied_info.fixed_head;
    }
  if (addr)
    {
      // record allocated chunk's type
      new_node = fixed_malloc (varied_info.node_token);
      if (new_node == NULL)
	{
	  // TODO: free addr
	  return NULL;
	}
      new_node->addr = addr;
      list_add (&new_node->list, target_head);
    }
  return addr;
}
