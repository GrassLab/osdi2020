#include "mem.h"

#include "math.h"
#include "type.h"
#include "uart.h"

buddy_t * buddy_pool[PAGE_MAX_ORDER];
buddy_t buddy_self[64];
uint64_t buddy_record_usage = 0;

// function that would be only used in this file
void push_buddy_list ( int buddy_pool_index, buddy_t * node );
void pop_buddy_list ( int buddy_pool_index, buddy_t * node );

void buddy_init ( )
{
    int i;

    for ( i = 0; i < PAGE_MAX_ORDER; i++ )
        buddy_pool[i] = NULL;

    buddy_pool[PAGE_MAX_ORDER - 1] = &buddy_self[0];
    buddy_self[0].self_index       = 0;
    buddy_self[0].used             = 0;
    buddy_self[0].addr             = MEM_LOW;
    buddy_self[0].pages            = pow ( 2, PAGE_MAX_ORDER - 1 );

    buddy_record_usage = 1;
}

void * allocate_free_mem ( int size_in_byte )
{
    int size_in_page;
    int buddy_pool_index;
    int buddy_self_index;
    int request_power;
    buddy_t * temp;

    size_in_page = size_in_byte >> PAGE_SIZE_OFFSET;

    if ( size_in_byte % PAGE_SIZE != 0 )
        size_in_page++;

    request_power = log2 ( round_to_next_power_2 ( size_in_page ) );

    temp = NULL;
    for ( buddy_pool_index = request_power; buddy_pool_index < PAGE_MAX_ORDER; buddy_pool_index++ )
    {
        if ( buddy_pool[buddy_pool_index] == NULL )
            continue;

        temp = buddy_pool[buddy_pool_index];
        while ( temp != NULL )
        {
            if ( temp->used )
            {
                temp = temp->next_sibling;
                continue;
            }
            else
            {
                break;
            }
        }

        if ( temp != NULL )
            break;
    }

    if ( temp == NULL )
    {
        uart_printf ( "No more available memory!!\n" );
        return NULL;
    }

    while ( ( temp->pages ) / 2 >= size_in_page && buddy_pool_index > 0 )
    {
        temp->used = 1;

        buddy_self_index = find_first_0_in_bit ( buddy_record_usage );
        buddy_record_usage |= 1UL << buddy_self_index;
        temp->left             = &buddy_self[buddy_self_index];
        temp->left->self_index = buddy_self_index;

        buddy_self_index = find_first_0_in_bit ( buddy_record_usage );
        buddy_record_usage |= 1UL << buddy_self_index;
        temp->right             = &buddy_self[buddy_self_index];
        temp->right->self_index = buddy_self_index;

        temp->left->parrent      = temp;
        temp->left->pages        = ( temp->pages ) / 2;
        temp->left->addr         = temp->addr;
        temp->left->used         = 0;
        temp->left->next_sibling = NULL;

        temp->right->parrent      = temp;
        temp->right->pages        = ( temp->pages ) / 2;
        temp->right->addr         = ( temp->addr ) + ( ( temp->pages / 2 ) * PAGE_SIZE );
        temp->right->used         = 0;
        temp->right->next_sibling = NULL;

        uart_printf ( "\033[0;31m[Split]\033[0m [addr: %d\tlen: %d\t] ==> [addr: %d\tlen: %d\t], [addr: %d\tlen: %d\t] \n", temp->addr, temp->pages * PAGE_SIZE, temp->left->addr,
                      temp->left->pages * PAGE_SIZE, temp->right->addr, temp->right->pages * PAGE_SIZE );

        push_buddy_list ( buddy_pool_index - 1, temp->left );
        push_buddy_list ( buddy_pool_index - 1, temp->right );

        buddy_pool_index--;

        temp = temp->left;
    }

    temp->used = 1;

    uart_printf ( "\033[0;32m[Allocated]\033[0m Request %d, Get %d | address: %d\n", size_in_byte, temp->pages * PAGE_SIZE, temp->addr );

    return (void *) ( temp->addr );
}

void free_mem ( void * addr )
{
    int i;
    buddy_t * temp = NULL;
    int flag;

    flag = 0;

    for ( i = 0; i < PAGE_MAX_ORDER && !flag; i++ )
    {
        temp = buddy_pool[i];

        while ( temp != 0 )
        {
            if ( temp->addr != ( (uint64_t) addr ) )
            {
                temp = temp->next_sibling;
                continue;
            }
            else
            {
                temp->used = 0;
                flag       = 1;
                break;
            }
        }
    }

    uart_printf ( "\033[0;32m[Free]\033[0m [addr: %d, len: %d]\n", addr, ( temp->pages ) * PAGE_SIZE );

    do
    {
        temp = temp->parrent;

        if ( temp->left->used == 0 && temp->right->used == 0 )
        {
            temp->used = 0;

            uart_printf ( "\033[0;31m[Merge]\033[0m [addr: %d\tlen: %d\t], [addr: %d\tlen: %d\t] ==> [addr: %d\tlen: %d\t]  \n", temp->left->addr, temp->left->pages * PAGE_SIZE, temp->right->addr,
                          temp->right->pages * PAGE_SIZE, temp->addr, temp->pages * PAGE_SIZE );

            pop_buddy_list ( i - 1, temp->left );
            pop_buddy_list ( i - 1, temp->right );
        }
        else
        {
            break;
        }

        i++;

    } while ( temp != &buddy_self[0] );
}

void push_buddy_list ( int buddy_pool_index, buddy_t * node )
{
    buddy_t * temp;

    if ( buddy_pool[buddy_pool_index] == NULL )
    {
        buddy_pool[buddy_pool_index] = node;
        return;
    }

    temp = buddy_pool[buddy_pool_index];

    while ( temp->next_sibling != NULL )
        temp = temp->next_sibling;

    temp->next_sibling = node;
}

void pop_buddy_list ( int buddy_pool_index, buddy_t * node )
{
    buddy_t * temp;

    node->used = 0;
    buddy_record_usage &= ~( 1UL << node->self_index );

    temp = buddy_pool[buddy_pool_index];

    if ( temp == node )
    {
        if ( temp->next_sibling == NULL )
            buddy_pool[buddy_pool_index] = NULL;
        else
            buddy_pool[buddy_pool_index] = temp->next_sibling;
        return;
    }

    while ( 1 )
    {
        if ( temp->next_sibling == node )
        {
            temp->next_sibling = node->next_sibling;
            break;
        }
        else
        {
            temp = temp->next_sibling;
        }
    }
}