#include "allocator.h"
#include "fs/fat32.h"
#include "mem.h"
#include "shell.h"
#include "uart.h"

int main ( )
{
    // set up serial console
    uart_init ( );

    uart_printf ( "Hello World\n" );
    uart_printf ( "\n\n\n" );

    buddy_init ( );
    allocator_init ( );

    fat32_init ( );

    // start shell
    shell_start ( );

    return 0;
}