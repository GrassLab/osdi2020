#include "allocator.h"
#include "mem.h"
#include "shell.h"
#include "uart.h"

int main ( )
{
    // set up serial console
    uart_init ( );

    buddy_init ( );

    // say hello
    uart_puts ( "Hello World!\n" );

    void * a = allocate_free_mem ( 5960 );
    void * b = allocate_free_mem ( 568 );
    void * c = allocate_free_mem ( 4097 );
    void * d = allocate_free_mem ( 1024 );

    free_mem ( a );
    free_mem ( b );
    free_mem ( c );
    free_mem ( d );

    allocator_t * bytes_30_allocator = register_fixed_size_allocator ( 30 );
    alloc ( bytes_30_allocator );

    allocator_t * bytes_60_allocator = register_fixed_size_allocator ( 60 );
    alloc ( bytes_60_allocator );

    alloc ( bytes_30_allocator );

    allocator_t * bytes_90_allocator = register_fixed_size_allocator ( 90 );
    alloc ( bytes_90_allocator );

    // start shell
    shell_start ( );

    return 0;
}