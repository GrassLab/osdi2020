#include "allocator.h"

#include "mem.h"
#include "type.h"
#include "uart.h"

allocator_t * register_fixed_size_allocator ( int size )
{
    allocator_t * allocator = (allocator_t *) allocate_free_mem ( PAGE_SIZE );

    allocator->allocate_status = 0;
    allocator->size            = size;
    allocator->start_addr      = (void *) ( ( intptr_t ) ( allocator ) + sizeof ( allocator_t ) );

    uart_printf ( "\033[0;33m[Allocator]\033[0m Fixed Size Allocator (%d), [addr: %d, len %d]\n", allocator, allocator->start_addr, size );

    return allocator;
}