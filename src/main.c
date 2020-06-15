#include "allocator.h"
#include "fs/fat32.h"
#include "mem.h"
#include "shell.h"
#include "uart.h"
#include "vfs.h"

int main ( )
{
    // set up serial console
    uart_init ( );

    uart_printf ( "\n\n\n===================\n" );

    buddy_init ( );
    allocator_init ( );

    fat32_init ( );

    struct file * a = vfs_open ( "hello", 0 );
    uart_printf ( ">> %d\n", a );

    struct file * b = vfs_open ( "test.txt", 0 );
    uart_printf ( ">> %d\n", b );

    char buf[100];
    vfs_read ( b, buf, 7 );
    uart_printf ( ">> %s\n", buf );

    vfs_read ( b, buf, 100 );
    uart_printf ( ">> %s\n", buf );

    vfs_write ( b, "Hello World\n", 12 );
    vfs_close ( b );

    b = vfs_open ( "test.txt", 0 );
    vfs_read ( b, buf, 100 );
    uart_printf ( ">> %s\n", buf );

    // start shell
    shell_start ( );

    return 0;
}