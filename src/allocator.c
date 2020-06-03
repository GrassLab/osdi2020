#include "allocator.h"

#include "math.h"
#include "mem.h"
#include "type.h"
#include "uart.h"

// function that only used in this file
int check_addr_belong_fixed_allocator ( fixed_allocator_t * allocator, void * address );

fixed_allocator_t * register_fixed_size_allocator ( int size )
{
    fixed_allocator_t * allocator = (fixed_allocator_t *) allocate_free_mem ( PAGE_SIZE );

    allocator->allocate_status = 0;
    allocator->size            = size;
    allocator->start_addr      = ( intptr_t ) ( allocator ) + sizeof ( fixed_allocator_t );

    uart_printf ( "\033[0;33m[Register Fixed Allocator %d]\033[0m\t[addr: %d, len %d]\n", allocator, allocator->start_addr, size );

    return allocator;
}

dynamic_allocator_t * register_varied_size_allocator ( )
{
    dynamic_allocator_t * allocator = (dynamic_allocator_t *) allocate_free_mem ( PAGE_SIZE );

    uart_printf ( "\033[0;33m[Register Varied Allocator %d]\033[0m\t[addr: %d, len %d]\n", allocator );

    allocator->byte_64_fixed  = register_fixed_size_allocator ( 64 );
    allocator->byte_128_fixed = register_fixed_size_allocator ( 128 );
    allocator->byte_256_fixed = register_fixed_size_allocator ( 256 );

    allocator->page_alloc_list            = (page_alloc_record_t *) ( ( intptr_t ) ( allocator ) + sizeof ( dynamic_allocator_t ) );
    allocator->page_alloc_list->page_addr = (intptr_t) NULL;
    allocator->page_alloc_list->next      = NULL;

    return allocator;
}

void * fixed_alloc ( fixed_allocator_t * allocator )
{
    int index;
    void * addr;

    index = find_first_0_in_bit ( allocator->allocate_status );

    allocator->allocate_status |= 1UL << index;

    addr = (void *) ( allocator->start_addr + ( allocator->size ) * index );

    uart_printf ( "\033[0;34m[Allocate From Fixed Allocator]\033[0m\t\t[index: %d addr: %d, len %d]\n", index, addr, ( allocator->size ) );

    return addr;
}

void fixed_free ( fixed_allocator_t * allocator, void * addr )
{
    int index;

    index = ( (intptr_t) addr - ( allocator->start_addr ) ) / allocator->size;

    allocator->allocate_status &= ~( 1UL << index );

    uart_printf ( "\033[0;34m[Free From Fixed Allocator]\033[0m\t\t[index: %d addr: %d, len %d]\n", index, addr, ( allocator->size ) );
}

void * dynamic_alloc ( dynamic_allocator_t * allocator, int size )
{
    page_alloc_record_t * temp;

    if ( size <= 64 )
    {
        uart_printf ( "\033[0;34m[Allocate From Varied Allocator]\033[0m\tRequest Size: %d,\tProxy to 64 allocator: %d\n", size, allocator->byte_64_fixed );
        return fixed_alloc ( allocator->byte_64_fixed );
    }
    else if ( size <= 128 )
    {
        uart_printf ( "\033[0;34m[Allocate From Varied Allocator]\033[0m\tRequest  Size: %d,\tProxy to 128 allocator: %d\n", size, allocator->byte_128_fixed );
        return fixed_alloc ( allocator->byte_128_fixed );
    }
    else if ( size <= 256 )
    {
        uart_printf ( "\033[0;34m[Allocate From Varied Allocator]\033[0m\tRequest  Size: %d,\tProxy to 256 allocator: %d\n", size, allocator->byte_256_fixed );
        return fixed_alloc ( allocator->byte_256_fixed );
    }
    else
    {
        uart_printf ( "\033[0;34m[Allocate From Varied Allocator]\033[0m\tRequest  Size: %d,\tProxy to Page Allocator\n", size );
        temp = allocator->page_alloc_list;

        while ( (void *) ( temp->page_addr ) != NULL )
        {
            temp = temp->next;
        }

        temp->page_addr = (intptr_t) allocate_free_mem ( size );

        temp->next            = (page_alloc_record_t *) ( (intptr_t) temp + sizeof ( page_alloc_record_t ) );
        temp->next->page_addr = (intptr_t) NULL;
        temp->next->next      = NULL;

        return (void *) ( temp->page_addr );
    }
}

void dynamic_free ( dynamic_allocator_t * allocator, void * addr )
{
    page_alloc_record_t * temp;

    if ( check_addr_belong_fixed_allocator ( allocator->byte_64_fixed, addr ) )
    {
        uart_printf ( "\033[0;34m[Free From Varied Allocator]\033[0m\t\taddr: %d, Proxy to 64 allocator\n", addr );
        fixed_free ( allocator->byte_64_fixed, addr );
    }
    else if ( check_addr_belong_fixed_allocator ( allocator->byte_128_fixed, addr ) )
    {
        uart_printf ( "\033[0;34m[Free From Varied Allocator]\033[0m\t\taddr: %d, Proxy to 128 allocator\n", addr );
        fixed_free ( allocator->byte_128_fixed, addr );
    }
    else if ( check_addr_belong_fixed_allocator ( allocator->byte_256_fixed, addr ) )
    {
        uart_printf ( "\033[0;34m[Free From Varied Allocator]\033[0m\t\taddr: %d, Proxy to 256 allocator\n", addr );
        fixed_free ( allocator->byte_256_fixed, addr );
    }
    else
    {
        temp = allocator->page_alloc_list;

        uart_printf ( "\033[0;34m[Free From Varied Allocator]\033[0m\t\taddr: %d, Proxy to Page allocator\n", addr );

        if ( temp != NULL && temp->page_addr == (intptr_t) addr )
        {
            free_mem ( addr );

            if ( temp->next == NULL )
            {
                allocator->page_alloc_list            = (page_alloc_record_t *) ( (intptr_t) allocator + sizeof ( dynamic_allocator_t ) );
                allocator->page_alloc_list->next      = NULL;
                allocator->page_alloc_list->page_addr = (intptr_t) NULL;
            }
            else
            {
                allocator->page_alloc_list = temp->next;
            }
        }
        else
        {
            while ( temp->next != NULL && temp->next->page_addr != (intptr_t) addr )
            {
                temp = temp->next;
            }

            if ( temp->next != NULL )
            {
                free_mem ( addr );

                temp->next = temp->next->next;

                if ( temp->next == NULL )
                {
                    temp->next            = (page_alloc_record_t *) ( (intptr_t) temp + sizeof ( page_alloc_record_t ) );
                    temp->next->page_addr = (intptr_t) NULL;
                    temp->next->next      = NULL;
                }
            }
        }
    }
}

int check_addr_belong_fixed_allocator ( fixed_allocator_t * allocator, void * address )
{
    intptr_t addr = (intptr_t) address;
    int index;

    if ( addr < allocator->start_addr )
        return 0;

    if ( ( addr - allocator->start_addr ) % ( allocator->size ) != 0 )
        return 0;

    index = ( addr - allocator->start_addr ) / ( allocator->size );

    if ( index < PAGE_MAX_ORDER )
        return 1;
    else
        return 0;
}