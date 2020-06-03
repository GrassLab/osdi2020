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

    fixed_allocator_t * bytes_30_allocator = register_fixed_size_allocator ( 30 );
    void * byte_30_fixed_a                 = fixed_alloc ( bytes_30_allocator );

    fixed_allocator_t * bytes_60_allocator = register_fixed_size_allocator ( 60 );
    void * byte_60_fixed_a                 = fixed_alloc ( bytes_60_allocator );

    void * byte_30_fixed_b = fixed_alloc ( bytes_30_allocator );

    fixed_allocator_t * bytes_90_allocator = register_fixed_size_allocator ( 90 );
    void * byte_90_fixed_a                 = fixed_alloc ( bytes_90_allocator );

    fixed_free ( bytes_30_allocator, byte_30_fixed_a );
    fixed_free ( bytes_60_allocator, byte_60_fixed_a );
    fixed_free ( bytes_30_allocator, byte_30_fixed_b );
    fixed_free ( bytes_90_allocator, byte_90_fixed_a );

    register_varied_size_allocator ( );

    // start shell
    shell_start ( );

    return 0;
}