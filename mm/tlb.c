#include <string.h>
#include <stddef.h>
#include <irq.h>
#include <sched.h>
#include "tlb.h"

void *
page_alloc_virt (size_t PGD, size_t virt_addr, size_t page_num, size_t attr)
{
  void *new;
  int retval;

  new = page_alloc (page_num);
  if (!new)
    return NULL;
  if (!virt_addr)
    {
      /* to prevent recursive with map_virt_to_phys
       * we need promise the page tables of this
       * virtual address are already exist
       */
      virt_addr = (size_t) new | KPGD;
    }
  retval = map_virt_to_phys (PGD, virt_addr, (size_t) new,
			     page_num * PAGE_SIZE, attr);
  if (retval)
    {
      page_free (new, page_num);
      return NULL;
    }
  bzero ((void *) virt_addr, page_num * PAGE_SIZE);
  return (void *) virt_addr;
}

void
page_free_virt (size_t PGD, size_t virt_addr, size_t page_num)
{
  int tlb_ind, page_ind;
  size_t *table;
  size_t i, virt;
  size_t *victim;
  virt = virt_addr;
  for (i = 0; i < page_num; ++i)
    {
      table = (size_t *) PGD;
      for (tlb_ind = 39; tlb_ind >= 21; tlb_ind -= 9)
	{
	  page_ind = (virt >> tlb_ind) & 0x1ff;
	  if (!table[page_ind])
	    {
	      // TODO: handle misusage but skip it now :p
	      return;
	    }
	  table = PD_DECODE (table[page_ind]);
	}
      page_ind = (virt >> tlb_ind) & 0x1ff;
      victim = (size_t *) ((size_t) PD_DECODE (table[page_ind]) & ~KPGD);
      page_free (victim, 1);
      table[page_ind] = 0;
      virt += PAGE_SIZE;
    }
}

int
unmap_virt (size_t PGD, size_t virt_addr, size_t size)
{
  size_t offset, page_ind;
  size_t virt;
  size_t *table;
  int tlb_ind;

  for (offset = 0; offset < size; offset += PAGE_SIZE)
    {
      virt = virt_addr + offset;
      table = (size_t *) PGD;
      for (tlb_ind = 39; tlb_ind >= 21; tlb_ind -= 9)
	{
	  page_ind = (virt >> tlb_ind) & 0x1ff;
	  if (!table[page_ind])
	    {
	      // TODO: handle misused
	      printf ("%s\r\n", "TODO: handle misused");
	      return 0;
	    }
	  table = PD_DECODE (table[page_ind]);
	}
      page_ind = (virt >> 12) & 0x1ff;
      if (!table[page_ind])
	{
	  // TODO: handle misused
	  printf ("%s\r\n", "TODO: handle misused");
	  return 0;
	}

      table[page_ind] = 0;
    }
  return 0;
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
	      // alloc new page for page table
	      table[page_ind] =
		pd_encode_table (page_alloc_virt
				 (KPGD, 0, 1, pd_encode_ram (0)));
	      if (table[page_ind] == pd_encode_table (0))
		return -1;
	    }
	  table = PD_DECODE (table[page_ind]);
	}
      page_ind = (virt >> 12) & 0x1ff;
      // allow reset attribute
      if (table[page_ind] && (table[page_ind] & ~0xfff) !=  phys)
	return -2;
      table[page_ind] = phys | attr;
      if (phys < USED_MEMSIZE)
	{
	  // setup page_struct
	  page_init (&page_pool[phys >> 12], PGD, virt);
	}
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
  for (i = 0x7e000; i < ((size_t) _kernel_end & ~KPGD); i += PAGE_SIZE)
    PTE[i >> 12] = pd_encode_ram ((size_t *) i);
  mmu_enable (PGD);
  // set used pages
  for (i = 0; i < 0x4000; i += PAGE_SIZE)
    page_init (&page_pool[i >> 12], KPGD, i | KPGD);
  for (i = 0x7e000; i < ((size_t) _kernel_end & ~KPGD); i += PAGE_SIZE)
    page_init (&page_pool[i >> 12], KPGD, i | KPGD);
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
 * NULL for alloc fail
 * Note: not initialize
 * TODO: quickly
 */
void *
page_alloc (size_t page_num)
{
  size_t i, cnt, target;
  critical_entry ();
  cnt = 0;
  for (i = 0; i < PAGE_POOL_SIZE; ++i)
    {
      if (!page_pool[i].in_used)
	{
	  if (cnt == 0)
	    {
	      target = i;
	      cnt = 1;
	    }
	  else
	    ++cnt;
	}
      else
	{
	  cnt = 0;
	}
      if (cnt == page_num)
	{
	  // just set in_used bit
	  for (i = 0; i < cnt; ++i)
	    page_init (&page_pool[target + i], 0, 0);
	  critical_exit ();
	  return (void *) (target << 12);
	}
    }
  // physical address 0x0 for kernel PGD
  // it is already in used and impossible alloc again
  // so return NULL for error
  critical_exit ();
  return NULL;
}

void
page_free (void *paddr, size_t page_num)
{
  struct page_struct *p;
  size_t i;
  critical_entry ();
  p = &page_pool[(size_t) paddr >> 12];
  for (i = 0; i < page_num; ++i)
    {
      p->in_used = 0;
      p->PGD = 0;
      p->virt_addr = 0;
      ++p;
    }
  critical_exit ();
}

void
page_init (struct page_struct *page, size_t PGD, size_t virt_addr)
{
  page->PGD = PGD;
  page->virt_addr = virt_addr;
  page->in_used = 1;
}

void
do_page_status (int *free, int *alloc)
{
  int i;
  *free = 0;
  *alloc = 0;
  for (i = 0; i < PAGE_POOL_SIZE; ++i)
    {
      if (page_pool[i].in_used)
	(*alloc)++;
      else
	(*free)++;
    }
}

void
sys_page_status (int *free, int *alloc)
{
  do_page_status (free, alloc);
}

void *
virt_to_phys (void *virt)
{
  size_t offset, page_ind;
  size_t *table;
  int tlb_ind;

  // select PGD
  if ((size_t) virt & KPGD)
    table = (size_t *) KPGD;
  else
    table = (size_t *) (current->ctx.PGD | KPGD);
  // search physical address
  for (tlb_ind = 39; tlb_ind >= 12; tlb_ind -= 9)
    {
      page_ind = ((size_t) virt >> tlb_ind) & 0x1ff;
      if (!table[page_ind])
	{
	  // TODO: handle no entry
	  printf ("%s\r\n", "TODO: handle no entry (virt_to_phys)");
	  return 0;
	}
      table = PD_DECODE (table[page_ind]);
    }
  table = (void *) ((size_t) table & ~KPGD);
  return (void *) ((size_t) table | ((size_t) virt & 0xfff));
}

void *
phys_to_virt (void *phys)
{
  if ((size_t) phys >= USED_MEMSIZE)
    return 0;
  return (void *) (page_pool[(size_t) phys / PAGE_SIZE].virt_addr |
		   ((size_t) phys & 0xfff));
}
