#include "sumhash.h"

/* Return 1 if match, 0 if mismatch */
int sumhash_check(uint8_t * address, uint16_t hash_value, unsigned length)
{
  uint16_t sum = 0;
  for(uint16_t idx = 0; idx < length; ++idx)
  {
#pragma GCC diagnostic ignored "-Wconversion"
    sum += *(address + idx);
  }
  if(sum == hash_value)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

