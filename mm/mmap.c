#include <string.h>
#include <sched.h>
#include "tlb.h"
#include "mmap.h"

void *
do_mmap (void *addr, size_t len, int prot,
	 int flags, void *file_start, int file_offset)
{
  int page_num;
  size_t attr;
  if (flags & MAP_FIXED)
    {
      if (addr == NULL || (size_t) addr % PAGE_SIZE > 0)
	return (void *) -1;
    }
  page_num = (len % PAGE_SIZE) ? 1 + len / PAGE_SIZE : len / PAGE_SIZE;
  // align address
  addr = (void *) ((size_t) addr & ~0xfff);
  // set attribute
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
  // allocate pages
  addr =
    page_alloc_virt (current->ctx.PGD | KPGD, (size_t) addr, page_num, attr);
  if (!addr)
    return (void *) -1;
  // record map info
  va_map_add ((size_t) addr, page_num * PAGE_SIZE);
  // copy file content
  if (flags & MAP_POPULATE)
    memcpy (addr, file_start + file_offset, len);
  return addr;
}

void *
sys_mmap (void *addr, size_t len, int prot,
	  int flags, void *file_start, int file_offset)
{
  return do_mmap (addr, len, prot, flags, file_start, file_offset);
}
