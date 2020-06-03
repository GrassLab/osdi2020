#include "allocator.h"

#include "math.h"
#include "mem.h"
#include "type.h"
#include "uart.h"

allocator_t * register_fixed_size_allocator ( int size )
{
    allocator_t * allocator = (allocator_t *) allocate_free_mem ( PAGE_SIZE );

    allocator->allocate_status = 0;
    allocator->size            = size;
    allocator->start_addr      = ( intptr_t ) ( allocator ) + sizeof ( allocator_t );

    uart_printf ( "\033[0;33m[Allocator]\033[0m Fixed Size Allocator (%d), [addr: %d, len %d]\n", allocator, allocator->start_addr, size );

    return allocator;
}

void * alloc ( allocator_t * allocator )
{
    int index;
    void * addr;

    index = find_first_0_in_bit ( allocator->allocate_status );

    allocator->allocate_status |= 1UL << index;

    addr = (void *) ( allocator->start_addr + ( allocator->size ) * index );

    uart_printf ( "\033[0;34m[Fixed Allocator]\033[0m [index: %d addr: %d, len %d]\n", index, addr, ( allocator->size ) );

    return addr;
}
