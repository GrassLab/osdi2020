#include "uart.h"
#include "str.h"
#include "mailbox.h"
#include "lfb.h"
#include "exc.h"
#include "syscall.h"
#include "tmpfs.h"
#include "buddy.h"

/*void test_case()
{
    struct file* a = vfs_open("hello", O_CREAT);
    struct file* b = vfs_open("world", O_CREAT);
    vfs_write(a, "Hello ", 6);
    vfs_write(b, "World!", 6);
    vfs_close(a);
    vfs_close(b);
    b = vfs_open("hello", 0);
    a = vfs_open("world", 0);
    int sz;
    sz = vfs_read(b, buf, 100);
    sz += vfs_read(a, buf + sz, 100);
    buf[sz] = '\0';
    printf("%s\n", buf); // should be Hello World!
}*/

void require2_testcase()
{
    struct file* a = vfs_open("hello", 0);
    if(a != 0)
    {
        uart_puts("require2_testcase: open null file failed\r\n");
        return;
    }
    a = vfs_open("hello", O_CREAT);
    if(a == 0)
    {
        uart_puts("require2_testcase: creat hello file failed\r\n");
        return;
    }
    if(vfs_close(a) == 0)
    {
        uart_puts("require2_testcase: close a file failed\r\n");
        return;
    }
    struct file* b = vfs_open("hello", 0);
    if(b == 0)
    {
        uart_puts("require2_testcase: open hello file failed\r\n");
        return;
    }
    if(vfs_close(b) == 0)
    {
        uart_puts("require2_testcase: close b file failed\r\n");
        return;
    }
    uart_puts("require 2 test finish\r\n");
}

void require3_testcase()
{
    char buf[100]={0};
    struct file* a = vfs_open("hello", O_CREAT);
    struct file* b = vfs_open("world", O_CREAT);
    vfs_write(a, "Hello ", 6);
    vfs_write(b, "World!", 6);
    vfs_close(a);
    vfs_close(b);
    b = vfs_open("hello", 0);
    a = vfs_open("world", 0);
    int sz;
    sz = vfs_read(b, buf, 100);
    //my_printf("len: %d", sz);
    sz += vfs_read(a, buf + sz, 100);
    buf[sz] = '\0';
    my_printf("%s\n", buf); // should be Hello World!
}


void main()
{
    uart_init();
    link_init();
    rootfs_init();
    require3_testcase();
	while(1);
	
}