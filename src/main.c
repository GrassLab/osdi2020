#include "allocator.h"
#include "mem.h"
#include "shell.h"
#include "tmpfs.h"
#include "uart.h"
#include "vfs.h"

void test1 ( );
void test2 ( );

int main ( )
{
    // set up serial console
    uart_init ( );

    buddy_init ( );
    allocator_init ( );

    tmpfs_init ( );

    uart_printf ( "###################\n" );
    uart_printf ( "### TEST_CASE 1 ###\n" );
    uart_printf ( "###################\n" );
    test1 ( );

    uart_printf ( "###################\n" );
    uart_printf ( "### TEST_CASE 2 ###\n" );
    uart_printf ( "###################\n" );
    test2 ( );

    // start shell
    shell_start ( );

    return 0;
}

void test1 ( )
{
    // first test case
    file_t * a = vfs_open ( "hello1", 0 );
    uart_printf ( "> file desriptor try to open file hello1: %d\n", a );

    a = vfs_open ( "hello1", O_CREAT );
    uart_printf ( "> file desriptor try to create file hello1: %d\n", a->dentry );
    vfs_close ( a );

    struct file * b = vfs_open ( "hello1", 0 );
    uart_printf ( "> file desriptor try to open file hello1: %d\n", b->dentry );
    vfs_close ( b );
}

void test2 ( )
{
    char buf[100];
    file_t * a = vfs_open ( "hello", O_CREAT );
    file_t * b = vfs_open ( "world", O_CREAT );
    vfs_write ( a, "Hello ", 6 );
    vfs_write ( b, "World!", 6 );
    vfs_close ( a );
    vfs_close ( b );
    a = vfs_open ( "hello", 0 );
    b = vfs_open ( "world", 0 );
    int sz;
    sz = vfs_read ( a, buf, 3 );
    sz += vfs_read ( a, buf + sz, sz );
    sz += vfs_read ( b, buf + sz, 100 );
    buf[sz] = '\0';
    uart_printf ( "> %s\n", buf );  // should be Hello World!
}