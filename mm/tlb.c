#include <stddef.h>
#include "tlb.h"

void
tlb_init ()
{
  size_t i;
  extern char _kernel_end[];
  // kernel
  // ffff000000000000-ffff000000200000 -> 000000000000-000000200000
  // already called setup_va, just setup page map
  PAGE_MAP_SET (0);
  PAGE_MAP_SET (0x1000);
  PAGE_MAP_SET (0x2000);
  PAGE_MAP_SET (0x3000);
  // start from kernel stack
  for (i = 0x7e000; i < ((size_t) _kernel_end & 0xffffffffffff); i += 0x1000)
    PAGE_MAP_SET (i);
  // peripheral
  // ffffaaaa00000000-ffffaaaa01000000 -> 00003f000000-000040000000
  // arm local
  // ffffaaaa01000000-ffffaaaa01040000 -> 000040000000-000040040000
}

/* Return a page address (physical address)
 * TODO: quickly
 */
void *
page_alloc ()
{
  size_t i;
  for (i = 0; !PAGE_MAP_EMPTY (i) && PAGE_MAP_VALID (i); i += 0x1000);
  if (!PAGE_MAP_VALID (i))
    return NULL;
  PAGE_MAP_SET (i);
  return (void *) i;
}

void
page_free (void *paddr)
{
  if (PAGE_MAP_VALID ((size_t) paddr))
    PAGE_MAP_CLR ((size_t) paddr);
}
