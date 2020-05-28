#ifndef FIXED
#define FIXED

#include <stddef.h>

struct fixed_token
{
  size_t id;
  void *chunks;
  size_t chunk_len;
  size_t chunk_size;
  char *chunk_bitmap;
};

struct fixed_struct
{
  struct fixed_token *tokens;
  size_t token_len;
};

void fixed_free (void *addr);
void *fixed_malloc (size_t token_id);
size_t token_register (size_t size);
#endif /* ifndef FIXED */
