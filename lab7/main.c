#include "uart.h"
#include "mystd.h"
#include "vfs.h"

void assert(bool res){
    if(res) uart_puts("[assert] success\n");
    else uart_puts("[assert] fail\n");
}

void main()
{
    uart_init();
    
    uart_puts("=========== create tmpfs ===========\n");

    struct filesystem tmpfs;
    register_filesystem(&tmpfs);

    uart_puts("\n=========== vfs open close ===========\n");
    struct file_descriptor* a = vfs_open("hello", 0);
    assert(a == NULL);
    a = vfs_open("hello", O_CREAT);
    assert(a != NULL);
    vfs_close(a);
    struct file_descriptor* b = vfs_open("hello", 0);
    assert(b != NULL);
    vfs_close(b);


    uart_puts("\n=========== vfs read write ===========\n");
    a = vfs_open("hello", O_CREAT);
    b = vfs_open("world", O_CREAT);
    vfs_write(a, "Hello ", 6);
    vfs_write(b, "World!", 6);
    vfs_close(a);
    vfs_close(b);
    b = vfs_open("hello", 0);
    a = vfs_open("world", 0);
    int sz;
    char buf[128] = {0};
    sz = vfs_read(b, buf, 100);
    sz += vfs_read(a, buf + sz, 100);
    buf[sz] = '\0';
    
    uart_puts(buf);
    uart_puts("\n");
}
