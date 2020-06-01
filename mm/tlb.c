#include <string.h>
#include <stddef.h>
#include <irq.h>
#include <sched.h>
#include <hardware.h>
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
      if (table[page_ind])
	{
	  if ((table[page_ind] & ~0xfff) != phys)
	    return -2;
	  else
	    {
	      // allow reset attribute
	      // break-before-make because we modify the tlb entry
	      table[page_ind] = 0;
	      asm volatile ("dsb ish\n"
			    "tlbi vmalle1is\n" "dsb ish\n" "isb\n");
	    }
	}
      table[page_ind] = phys | attr;
      if (phys < PHYS_MEM_MAX)
	{
	  // setup page_struct
	  page_init (&page_pool[phys >> 12], PGD, virt);
	}
    }
  return 0;
}

static void
page_pool_init ()
{
  extern char _kernel_end[];
  size_t page_num_max;
  size_t page_pool_size;
  size_t mem_size;
  size_t kernel_end;
  size_t i;

  mem_size = hardware_info_memory_size ();
  page_num_max = mem_size / PAGE_SIZE;
  page_pool_size = page_num_max * sizeof (struct page_struct);
  PAGE_POOL_LEN = page_num_max;
  kernel_end = (size_t) _kernel_end;
  if (kernel_end & (PAGE_SIZE - 1))
    kernel_end = (kernel_end & ~(PAGE_SIZE - 1)) + PAGE_SIZE;
  if (mem_size - kernel_end < page_pool_size)
    {
      // impossible
      while (1);
    }
  page_pool = (struct page_struct *) kernel_end;
  bzero (page_pool, page_pool_size);
  for (i = kernel_end; i < (size_t) page_pool + page_pool_size;
       i += PAGE_SIZE)
    page_init (&page_pool[(i - KPGD) / PAGE_SIZE], KPGD, i);
}

static void
page_table_init ()
{
  size_t mem_size;
  size_t i;
  size_t *table;
  size_t tlb_ind, page_ind;
  size_t virt;

  mem_size = hardware_info_memory_size ();

  bzero ((void *) KPGD, PAGE_SIZE);
  for (i = 0; i < mem_size; i += 2 * M)
    {
      table = (void *) KPGD;
      virt = i | KPGD;
      for (tlb_ind = 39; tlb_ind > 12; tlb_ind -= 9)
	{
	  page_ind = ((size_t) virt >> tlb_ind) & 0x1ff;
	  if (!table[page_ind])
	    {
	      table[page_ind] = pd_encode_table (page_alloc (1));
	      if (table[page_ind] == pd_encode_table (0))
		{
		  while (1);
		}
	      bzero (PD_DECODE (table[page_ind]), PAGE_SIZE);
	    }
	  table = PD_DECODE (table[page_ind]);
	}
    }
}

/**
 * allocate page table by memory size
 * page_table mapping rule:
 * phys = virt[47:12] << 12
 * ex:
 * 0x3000 = 0xffff000000003000[47:12] << 12
 */
void
tlb_init ()
{
  extern char _kernel_end[];
  extern char _kernel_start[];
  size_t *PGD, *PUD;
  size_t i;
  size_t kernel_start;
  size_t kernel_end;
  size_t mem_size;
  struct page_struct *page;
  // setup 2 level tlb for startup allocator
  // 0x1000 for temp PGD, we will set PGD back to 0 later
  PGD = (size_t *) 0x1000;
  PUD = (size_t *) 0x2000;
  bzero (PGD, 0x2000);
  PGD[0] = pd_encode_table (PUD);
  PUD[0] = pd_encode_block (0);
  mmu_enable (PGD);
  // allocate page_pool
  page_pool_init ();
  // setup used pages
  // we need 0 for kernel PGD, so set in used first
  page_init (&page_pool[0], KPGD, (size_t) KPGD);
  page_init (&page_pool[1], KPGD, (size_t) PGD);
  page_init (&page_pool[2], KPGD, (size_t) PUD);
  // setup used kernel
  kernel_start = (size_t) _kernel_start - STACK_SIZE;
  kernel_end = (size_t) _kernel_end;
  for (i = kernel_start; i < kernel_end; i += PAGE_SIZE)
    page_init (&page_pool[(i - KPGD) / PAGE_SIZE], KPGD, i);
  // setup page tables
  page_table_init ();
  // mapping in used pages
  mem_size = hardware_info_memory_size ();
  for (i = 0; i < mem_size; i += PAGE_SIZE)
    {
      page = &page_pool[i / PAGE_SIZE];
      if (page->in_used)
	{
	  map_virt_to_phys (KPGD, i | KPGD, i, PAGE_SIZE, pd_encode_ram (0));
	  page_init (page, KPGD, i | KPGD);
	}
    }
  // switch back to KPGD
  asm volatile ("dsb ish\n" "msr ttbr0_el1, xzr\n" "msr ttbr1_el1, xzr\n"
		"tlbi vmalle1is\n" "dsb ish\n" "isb\n");
  // free temp PGD PUD
  page_free_virt (KPGD, 0x1000 | KPGD, 2);
  // TODO: preserve direct mapping address (ffff000000000000 -> ffff000_MEM_SIZE)
  // peripheral
  // ffff00003f000000-ffff000040000000 -> 00003f000000-000040000000
  map_virt_to_phys (KPGD, 0xffff00003f000000, 0x3f000000, 0x1000000,
		    pd_encode_peripheral (0));
  // arm local
  // ffff000040000000-ffff000040040000 -> 000040000000-000040040000
  map_virt_to_phys (KPGD, 0xffff000040000000, 0x40000000, 0x40000,
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
  for (i = 0; i < PAGE_POOL_LEN; ++i)
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
  size_t i;
  *free = 0;
  *alloc = 0;
  for (i = 0; i < PAGE_POOL_LEN; ++i)
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
  size_t page_ind;
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
  if ((size_t) phys >= PHYS_MEM_MAX)
    return 0;
  return (void *) (page_pool[(size_t) phys / PAGE_SIZE].virt_addr |
		   ((size_t) phys & 0xfff));
}
