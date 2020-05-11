#include <string.h>
#include <stddef.h>
#include "tlb.h"

/* assume we just need 2MB memory
 * PGD, PUD, PMD, PTE are physical address
 * mapping rule:
 * phys = virt[47:12] << 12
 * ex:
 * 0x3000 = 0xffff000000003000[47:12] << 12
 */
void
tlb_init ()
{
  extern char _kernel_end[];
  size_t *PGD, *PUD, *PMD, *PTE;
  size_t i;
  bzero (0, 0x4000);
  PGD = 0;
  PUD = (size_t *) 0x1000;
  PMD = (size_t *) 0x2000;
  PTE = (size_t *) 0x3000;
  // PGD -> PUD -> PMD -> PTE
  PGD[0] = pd_encode_table (PUD);
  PUD[0] = pd_encode_table (PMD);
  PMD[0] = pd_encode_table (PTE);
  // page table
  // ffff000000000000-ffff000000004000 -> 00000000-00004000
  PTE[0] = pd_encode_ram (PGD);
  PTE[1] = pd_encode_ram (PUD);
  PTE[2] = pd_encode_ram (PMD);
  PTE[3] = pd_encode_ram (PTE);
  // kernel
  // start from kernel stack
  for (i = 0x7e000; i < ((size_t) _kernel_end & 0xffffffffffff); i += 0x1000)
    PTE[i >> 12] = pd_encode_ram ((size_t *) i);
  mmu_enable (PGD);
  // set used pages
  PAGE_MAP_SET (0);
  PAGE_MAP_SET (0x1000);
  PAGE_MAP_SET (0x2000);
  PAGE_MAP_SET (0x3000);
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
  bzero ((void *) i, 0x1000);
  return (void *) i;
}

void
page_free (void *paddr)
{
  if (PAGE_MAP_VALID ((size_t) paddr))
    PAGE_MAP_CLR ((size_t) paddr);
}
