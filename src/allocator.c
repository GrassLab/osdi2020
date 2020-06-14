#include "allocator.h"

#include "math.h"
#include "mem.h"
#include "type.h"
#include "uart.h"

// global variable only used in this file
dynamic_allocator_t * DEFAULT_ALLOCATOR;

// function that only used in this file
int check_addr_belong_fixed_allocator ( fixed_allocator_t * allocator, void * address );

void allocator_init ( )
{
    DEFAULT_ALLOCATOR = register_varied_size_allocator ( );
}

void * kmalloc ( int size )
{
    return dynamic_alloc ( DEFAULT_ALLOCATOR, size );
}

void kfree ( void * addr )
{
    dynamic_free ( DEFAULT_ALLOCATOR, addr );
}

fixed_allocator_t * register_fixed_size_allocator ( int size )
{
    fixed_allocator_t * allocator = (fixed_allocator_t *) allocate_free_mem ( PAGE_SIZE );

    allocator->allocate_status = 0;
    allocator->size            = size;
    allocator->start_addr      = ( intptr_t ) ( allocator ) + sizeof ( fixed_allocator_t );
    allocator->next            = NULL;
    allocator->prev            = NULL;

    return allocator;
}

dynamic_allocator_t * register_varied_size_allocator ( )
{
    dynamic_allocator_t * allocator = (dynamic_allocator_t *) allocate_free_mem ( PAGE_SIZE );

    allocator->byte_16_fixed  = register_fixed_size_allocator ( 16 );
    allocator->byte_32_fixed  = register_fixed_size_allocator ( 32 );
    allocator->byte_64_fixed  = register_fixed_size_allocator ( 64 );
    allocator->byte_128_fixed = register_fixed_size_allocator ( 128 );
    allocator->byte_256_fixed = register_fixed_size_allocator ( 256 );
    allocator->byte_512_fixed = register_fixed_size_allocator ( 512 );

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

    if ( index == -1 || index * allocator->size >= PAGE_SIZE )
    {
        if ( allocator->next == NULL )
        {
            allocator->next       = register_fixed_size_allocator ( allocator->size );
            allocator->next->prev = allocator;
        }

        return fixed_alloc ( allocator->next );
    }

    allocator->allocate_status |= 1UL << index;

    addr = (void *) ( allocator->start_addr + ( allocator->size ) * index );

    return addr;
}

void fixed_free ( fixed_allocator_t * allocator, void * addr )
{
    int index;
    intptr_t address = (intptr_t) addr;

    while ( allocator != NULL )
    {
        index = ( address - ( allocator->start_addr ) ) / allocator->size;

        if ( address < allocator->start_addr || ( address - ( allocator->start_addr ) ) % allocator->size != 0 || index > MAX_FIXED_ALLOCATOR_ENTITY )
            allocator = allocator->next;
        else
            break;
    }

    if ( allocator == NULL )
        return;

    allocator->allocate_status &= ~( 1UL << index );

    if ( allocator->allocate_status == 0 && allocator->prev != NULL )
    {
        allocator->prev->next = allocator->next;
        free_mem ( allocator );
    }
}

void * dynamic_alloc ( dynamic_allocator_t * allocator, int size )
{
    page_alloc_record_t * temp;

    if ( size <= 16 )
    {
        return fixed_alloc ( allocator->byte_16_fixed );
    }
    else if ( size <= 32 )
    {
        return fixed_alloc ( allocator->byte_32_fixed );
    }
    else if ( size <= 64 )
    {
        return fixed_alloc ( allocator->byte_64_fixed );
    }
    else if ( size <= 128 )
    {
        return fixed_alloc ( allocator->byte_128_fixed );
    }
    else if ( size <= 256 )
    {
        return fixed_alloc ( allocator->byte_256_fixed );
    }
    else if ( size <= 512 )
    {
        return fixed_alloc ( allocator->byte_512_fixed );
    }
    else
    {
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

    if ( check_addr_belong_fixed_allocator ( allocator->byte_16_fixed, addr ) )
    {
        fixed_free ( allocator->byte_64_fixed, addr );
    }
    else if ( check_addr_belong_fixed_allocator ( allocator->byte_32_fixed, addr ) )
    {
        fixed_free ( allocator->byte_64_fixed, addr );
    }
    else if ( check_addr_belong_fixed_allocator ( allocator->byte_64_fixed, addr ) )
    {
        fixed_free ( allocator->byte_64_fixed, addr );
    }
    else if ( check_addr_belong_fixed_allocator ( allocator->byte_128_fixed, addr ) )
    {
        fixed_free ( allocator->byte_128_fixed, addr );
    }
    else if ( check_addr_belong_fixed_allocator ( allocator->byte_256_fixed, addr ) )
    {
        fixed_free ( allocator->byte_256_fixed, addr );
    }
    else if ( check_addr_belong_fixed_allocator ( allocator->byte_512_fixed, addr ) )
    {
        fixed_free ( allocator->byte_512_fixed, addr );
    }
    else
    {
        temp = allocator->page_alloc_list;

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

    if ( allocator == NULL )
        return 0;

    if ( addr < allocator->start_addr )
    {
        return check_addr_belong_fixed_allocator ( allocator->next, address );
    }

    if ( ( addr - allocator->start_addr ) % ( allocator->size ) != 0 )
    {
        return check_addr_belong_fixed_allocator ( allocator->next, address );
    }

    index = ( addr - allocator->start_addr ) / ( allocator->size );

    if ( index < PAGE_MAX_ORDER )
        return 1;
    else
        return check_addr_belong_fixed_allocator ( allocator->next, address );
}