#include "allocator.h"
#include "mem.h"
#include "shell.h"
#include "uart.h"

int main ( )
{
    // set up serial console
    uart_init ( );

    buddy_init ( );
    allocator_init ( );

    // say hello
    uart_puts ( "Hello World!\n" );

    // start shell
    shell_start ( );

    return 0;
}