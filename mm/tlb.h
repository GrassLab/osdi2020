#ifndef TLB
#define TLB

#define K 1024
#define M (K * K)
#define G (M * K)
#define PAGE_SIZE (4 * K)
#define PI_MEMSIZE (1 * G)
#define PAGE_MAP_SIZE ((2 * M) / PAGE_SIZE / 8)

char page_map[PAGE_MAP_SIZE];

#endif /* ifndef TLB */
