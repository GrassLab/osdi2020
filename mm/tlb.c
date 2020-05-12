#include <string.h>
#include <stddef.h>
#include "tlb.h"

void *
page_alloc_virt (size_t virt_addr)
{
  void *new;
  size_t *PTE;
  int retval;

  new = page_alloc ();
  if (!new)
    return NULL;
  if (!virt_addr)
    {
      /* to prevent recursive with map_virt_to_phys
       * we need promise the page tables of this
       * virtual address are already exist
       */
      virt_addr = (size_t) new | 0xffff000000000000;
    }
  retval = map_virt_to_phys (KPGD, virt_addr, (size_t) new,
			     PAGE_SIZE, pd_encode_ram (0));
  if (retval)
    {
      page_free (new);
      return NULL;
    }
  bzero ((void *) virt_addr, PAGE_SIZE);
  return (void *) virt_addr;
}

/* Return -1 for memory not enough
 * -2 for conflicted virtual address
 */
int
map_virt_to_phys (size_t PGD, size_t virt_addr, size_t phys_addr,
		  size_t size, size_t attr)
{
  size_t offset, page_ind;
  size_t virt, phys;
  size_t *table;
  int tlb_ind;

  for (offset = 0; offset < size; offset += PAGE_SIZE)
    {
      virt = virt_addr + offset;
      phys = phys_addr + offset;
      table = (size_t *) PGD;
      for (tlb_ind = 39; tlb_ind >= 21; tlb_ind -= 9)
	{
	  page_ind = (virt >> tlb_ind) & 0x1ff;
	  if (!table[page_ind])
	    {
	      table[page_ind] = pd_encode_table (page_alloc_virt (0));
	      if (table[page_ind] == pd_encode_table (0))
		return -1;
	    }
	  table = PD_DECODE (table[page_ind]);
	}
      page_ind = (virt >> 12) & 0x1ff;
      if (table[page_ind])
	return -2;
      table[page_ind] = phys | attr;
    }
  return 0;
}

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
  // setup default page tables to prevent recursive
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
  map_virt_to_phys (KPGD, 0xffffaaaa00000000, 0x3f000000, 0x1000000,
		    pd_encode_peripheral (0));
  // arm local
  // ffffaaaa01000000-ffffaaaa01040000 -> 000040000000-000040040000
  map_virt_to_phys (KPGD, 0xffffaaaa01000000, 0x40000000, 0x40000,
		    pd_encode_peripheral (0));
}

/* Return a page address (physical address)
 * Note: not initialize
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
