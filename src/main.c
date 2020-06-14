#include "allocator.h"
#include "mem.h"
#include "shell.h"
#include "tmpfs.h"
#include "uart.h"
#include "vfs.h"

int main ( )
{
    // set up serial console
    uart_init ( );

    buddy_init ( );
    allocator_init ( );

    tmpfs_init ( );

    // say hello
    uart_puts ( "Hello World!\n" );

    // start shell
    shell_start ( );

    return 0;
}