#include "allocator.h"
#include "mem.h"
#include "shell.h"
#include "tmpfs.h"
#include "uart.h"

int main ( )
{
    // set up serial console
    uart_init ( );

    buddy_init ( );
    allocator_init ( );

    tmpfs_init ( );

    // start shell
    shell_start ( );

    return 0;
}