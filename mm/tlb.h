#ifndef TLB
#define TLB

#include <stddef.h>

#define K 1024
#define M (K * K)
#define G (M * K)
#define PAGE_SIZE (4 * K)
#define PI_MEMSIZE (1 * G)
#define USED_MEMSIZE (2 * M)
#define PAGE_POOL_SIZE (USED_MEMSIZE / PAGE_SIZE)
#define KPGD 0xffff000000000000
#define INVALID_PHYS_ADDR ((void *) 0xaaaaaaaaaaaaaaaa)
#define INVALID_VIRT_ADDR ((void *) 0x5555555555555555)

#define PD_DECODE(pd) ((size_t *)((size_t) pd & 0xfffffffff000 | 0xffff000000000000))
#define PD_RW (0x1 << 6)
#define PD_RO (0x3 << 6)
#define PD_XN (0x1 << 54)

struct page_struct
{
  char in_used;
  size_t PGD;
  size_t virt_addr;
} page_pool[PAGE_POOL_SIZE];

void page_init (struct page_struct *page, size_t PGD, size_t virt_addr);
void *page_alloc (size_t page_num);
void *page_alloc_virt (size_t PGD, size_t virt_addr, size_t page_num);
void page_free (void *paddr, size_t page_num);
void page_free_virt (size_t PGD, size_t virt_addr, size_t page_num);
void tlb_init ();
int map_virt_to_phys (size_t PGD, size_t virt_addr, size_t phys_addr,
		      size_t size, size_t attr);
extern size_t pd_encode_table (size_t *page_table);
extern size_t pd_encode_ram (size_t *addr);
extern size_t pd_encode_peripheral (size_t *addr);
extern void mmu_enable (size_t *PGD);

#endif /* ifndef TLB */
