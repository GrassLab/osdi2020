#include "allocator.h"

#include "math.h"
#include "mem.h"
#include "type.h"
#include "uart.h"

fixed_allocator_t * register_fixed_size_allocator ( int size )
{
    fixed_allocator_t * allocator = (fixed_allocator_t *) allocate_free_mem ( PAGE_SIZE );

    allocator->allocate_status = 0;
    allocator->size            = size;
    allocator->start_addr      = ( intptr_t ) ( allocator ) + sizeof ( fixed_allocator_t );

    uart_printf ( "\033[0;33m[Register Fixed Allocator %d]\033[0m [addr: %d, len %d]\n", allocator, allocator->start_addr, size );

    return allocator;
}

dynamic_allocator_t * register_varied_size_allocator ( )
{
    dynamic_allocator_t * allocator = (dynamic_allocator_t *) allocate_free_mem ( PAGE_SIZE );

    uart_printf ( "\033[0;33m[Register Varied Allocator %d]\033[0m [addr: %d, len %d]\n", allocator );

    allocator->byte_64_fixed  = register_fixed_size_allocator ( 64 );
    allocator->byte_128_fixed = register_fixed_size_allocator ( 128 );
    allocator->byte_256_fixed = register_fixed_size_allocator ( 256 );

    allocator->page_alloc_list = NULL;

    return allocator;
}

void * fixed_alloc ( fixed_allocator_t * allocator )
{
    int index;
    void * addr;

    index = find_first_0_in_bit ( allocator->allocate_status );

    allocator->allocate_status |= 1UL << index;

    addr = (void *) ( allocator->start_addr + ( allocator->size ) * index );

    uart_printf ( "\033[0;34m[Allocate From Fixed Allocator]\033[0m [index: %d addr: %d, len %d]\n", index, addr, ( allocator->size ) );

    return addr;
}

void fixed_free ( fixed_allocator_t * allocator, void * addr )
{
    int index;

    index = ( (intptr_t) addr - ( allocator->start_addr ) ) / allocator->size;

    allocator->allocate_status &= ~( 1UL << index );

    uart_printf ( "\033[0;34m[Free From Fixed Allocator]\033[0m [index: %d addr: %d, len %d]\n", index, addr, ( allocator->size ) );
}
