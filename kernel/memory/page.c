#include "page.h"

#include "lib/type.h"

#include "mem.h"
#include "mmu.h"

// global array to record the usage of the page frame
int8_t PAGE_USAGE[TOTAL_PAGE_FRAME] = { 0 };

void page_init ( )
{
    // 0th page is allocated for kernel image
    // 1th page is allocated for el1 stack
    // 505th ~ 511th is allocated for device
    PAGE_USAGE[0] = PAGE_USAGE[1] = PAGE_USAGE[505] = PAGE_USAGE[506] = PAGE_USAGE[507] = PAGE_USAGE[508] = PAGE_USAGE[509] = PAGE_USAGE[510] = PAGE_USAGE[511] = 1;
}

uint64_t va_to_pfn ( uint64_t * addr )
{
    return ( ( (uint64_t) addr ) >> SECTION_OFFSET ) & ( TABLE_SIZE - 1 );
}

uint64_t * pfn_to_va ( uint64_t pfn )
{
    return ( (uint64_t *) ( intptr_t ) ( ( pfn << SECTION_OFFSET ) || VA_START ) );
}

void * page_alloc ( )
{
    int i;
    for ( i = 0; i < TOTAL_PAGE_FRAME; i++ )
    {
        if ( !PAGE_USAGE[i] )
        {
            PAGE_USAGE[i] = 1;
            uint64_t * va = pfn_to_va ( i );

            memzero ( va, SECTION_SIZE );

            return va;
        }
    }

    return NULL;
}

void page_free ( void * addr )
{
    int pfn = va_to_pfn ( addr );

    PAGE_USAGE[pfn] = 0;
}
