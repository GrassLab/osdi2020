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

    // start shell
    shell_start ( );

    return 0;
}