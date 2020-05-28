#include "buddy.h"
#include "fixed.h"
#include "tlb.h"

static struct fixed_struct fixed_info;

int
fixed_init ()
{
  fixed_info.tokens = buddy_malloc (sizeof (struct fixed_token));
  if (fixed_info.tokens == NULL)
    return 1;
  fixed_info.token_len = 1;
  return 0;
}

int
tokens_expand ()
{
  fixed_info.token_len *= 2;
  fixed_info.tokens =
    buddy_realloc (fixed_info.tokens,
		   fixed_info.token_len * sizeof (struct fixed_token));
  if (fixed_info.tokens == NULL)
    return 1;
  return 0;
}

int
chunks_expand (struct fixed_token *token)
{
  token->chunk_len *= 2;
  token->chunks = buddy_realloc (token->chunks,
				 token->chunk_len * token->chunk_size);
  if (token->chunks == NULL)
    return 1;
  token->chunk_bitmap = buddy_realloc (token->chunk_bitmap, token->chunk_len);
  if (token->chunk_bitmap == NULL)
    return 1;
  return 0;
}

/**
 * Return token for success
 * Return 0 for fail
 */
size_t
token_register (size_t size)
{
  size_t i;
  struct fixed_token *token;
  if (fixed_info.tokens == NULL)
    if (fixed_init ())
      return 0;
  // find free token
  for (i = 0; i < fixed_info.token_len; ++i)
    {
      if (fixed_info.tokens[i].id == 0)
	break;
    }
  if (i == fixed_info.token_len)
    if (tokens_expand ())
      return 0;
  // token init
  token = &fixed_info.tokens[i];
  token->chunk_size = size;
  token->chunk_len = 1;
  token->chunks = buddy_malloc (token->chunk_size);
  if (token->chunks == NULL)
    return 0;
  token->chunk_bitmap = buddy_malloc (token->chunk_len);
  if (token->chunk_bitmap == NULL)
    return 0;
  token->id = i + 1;
  return token->id;
}

void *
fixed_malloc (size_t token_id)
{
  size_t i;
  struct fixed_token *token;

  if (fixed_info.tokens == NULL)
    if (fixed_init ())
      return NULL;
  if (token_id - 1 >= fixed_info.token_len)
    return NULL;
  token = &fixed_info.tokens[token_id - 1];
  if (token->id == 0)
    return NULL;
  // find free chunk
  for (i = 0; i < token->chunk_len; ++i)
    if (token->chunk_bitmap[i] == 0)
      break;
  if (i == token->chunk_len)
    if (chunks_expand (token))
      return NULL;
  // set inused
  token->chunk_bitmap[i] = 1;
  return token->chunks + i * token->chunk_size;
}
