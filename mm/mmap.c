#include <string.h>
#include <sched.h>
#include "tlb.h"
#include "mmap.h"

void *
do_mmap (void *addr, size_t len, int prot,
	 int flags, void *file_start, int file_offset)
{
  int page_num;
  int i;
  size_t attr;
  if (flags & MAP_FIXED)
    {
      if (addr == NULL || (size_t) addr % PAGE_SIZE > 0)
	return (void *) -1;
    }
  attr = pd_encode_ram (0);
  if (prot & PROT_READ)
    {
      if (prot & PROT_WRITE)
	attr |= PD_RW;
      else
	attr |= PD_RO;
    }
  if (!(prot & PROT_EXEC))
    attr |= PD_XN;
  page_num = (len % PAGE_SIZE) ? 1 + len / PAGE_SIZE : len / PAGE_SIZE;
  // align address
  addr = (void *) ((size_t) addr & ~0xfff);
  if (addr == NULL)
    {
      // find possible address
      for (i = 0; i < VA_MAP_SIZE; ++i)
	if (current->va_maps[i].size == 0)
	  break;
      addr = (size_t *) ((current->task_id << 30) | (i << 21));
    }
  // allocate pages
  addr =
    page_alloc_virt (current->ctx.PGD | KPGD, (size_t) addr, page_num,
		     pd_encode_ram (0));
  if (!addr)
    return (void *) -1;
  // copy file content
  if (flags & MAP_POPULATE)
    memcpy (addr, file_start + file_offset, len);
  // set attribute
  map_virt_to_phys (current->ctx.PGD | KPGD, (size_t) addr,
		    (size_t) virt_to_phys (addr), page_num * PAGE_SIZE, attr);
  // record map info
  va_map_add ((size_t) addr, page_num * PAGE_SIZE);
  return addr;
}

void *
sys_mmap (void *addr, size_t len, int prot,
	  int flags, void *file_start, int file_offset)
{
  return do_mmap (addr, len, prot, flags, file_start, file_offset);
}
