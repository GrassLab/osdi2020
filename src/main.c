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

    allocate_free_mem ( 5960 );
    allocate_free_mem ( 568 );
    allocate_free_mem ( 4097 );
    allocate_free_mem ( 1024 );

    // start shell
    shell_start ( );

    return 0;
}