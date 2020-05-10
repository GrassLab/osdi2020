#ifndef TLB
#define TLB

#define K 1024
#define M (K * K)
#define G (M * K)
#define PAGE_SIZE (4 * K)
#define PI_MEMSIZE (1 * G)
#define PAGE_MAP_SIZE ((2 * M) / PAGE_SIZE / 8)

// TODO: atomic
#define PAGE_MAP_SET(paddr) ({page_map[paddr >> 12 >> 3] |= 1 << ((paddr >> 12) & 0x7);})
#define PAGE_MAP_CLR(paddr) ({page_map[paddr >> 12 >> 3] &= ~(1 << ((paddr >> 12) & 0x7));})
#define PAGE_MAP_EMPTY(paddr) (!(page_map[paddr >> 12 >> 3] & (1 << ((paddr >> 12) & 0x7))))
#define PAGE_MAP_VALID(paddr) ((paddr >> 12 >> 3) < PAGE_MAP_SIZE)

char page_map[PAGE_MAP_SIZE];
void *page_alloc ();
void page_free (void *paddr);

#endif /* ifndef TLB */
