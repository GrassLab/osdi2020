#ifndef MEM_H
#define MEM_H

#include "gpio.h"
#include "type.h"

#define MEM_LOW  ( ( uint64_t ) ( (volatile unsigned int *) ( 0x90000 ) ) )
#define MEM_HIGH MMIO_BASE

#define PAGE_SIZE_OFFSET 12
#define PAGE_SIZE        ( 1 << PAGE_SIZE_OFFSET )
#define PAGE_MAX_ORDER   11

#define AVAILABE_MEMORY ( 1 << 22 )  // only used 4M in demo
#define AVAILABE_PAGES  ( AVAILABE_MEMORY / PAGE_SIZE )

typedef struct buddy
{
    int used;
    uint64_t addr;
    int pages; /* occupy how many pages */
    int self_index;
    struct buddy * next_sibling;
    struct buddy * left;
    struct buddy * right;
    struct buddy * parrent;

} buddy_t;

void buddy_init ( );
void * allocate_free_mem ( int size_in_byte );
void free_mem ( void * addr );

#endif