#include <stdint.h>
#ifndef __SLAB_H__
#define __SLAB_H__

/* Assume minimal allocation size is 8bytes */
/* 4KB can allocate 512 * 8 bytes */
/* 512 / 32 = 16 */
#define SLAB_USED_BIT_ARRAY_SIZE 16
struct slab_struct
{
  struct slab_struct * next_ptr;
  uint32_t used_bit_array[SLAB_USED_BIT_ARRAY_SIZE];
  uint64_t object_size;
  uint64_t * va;
};

/* sizeof(slab_struct) == 80 bytes, 4KB can contain 51 */
#define SLAB_NODE_POOL_SIZE 8

uint64_t slab_regist(unsigned bytes);
uint64_t * slab_allocate(uint64_t token);
void slab_free(uint64_t token, uint64_t * va);

struct slab_struct * slab_find_or_create_slab(unsigned bytes);
struct slab_struct * slab_allocate_slab(void);
void slab_insert_slab(struct slab_struct * node);

#endif

