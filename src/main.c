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
    void * byte_30_fixed_a           = alloc ( bytes_30_allocator );

    allocator_t * bytes_60_allocator = register_fixed_size_allocator ( 60 );
    void * byte_60_fixed_a           = alloc ( bytes_60_allocator );

    void * byte_30_fixed_b = alloc ( bytes_30_allocator );

    allocator_t * bytes_90_allocator = register_fixed_size_allocator ( 90 );
    void * byte_90_fixed_a           = alloc ( bytes_90_allocator );

    free ( bytes_30_allocator, byte_30_fixed_a );
    free ( bytes_60_allocator, byte_60_fixed_a );
    free ( bytes_30_allocator, byte_30_fixed_b );
    free ( bytes_90_allocator, byte_90_fixed_a );


    // start shell
    shell_start ( );

    return 0;
}