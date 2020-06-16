#include "mm.h"
#include "system.h"
#include "tmpfs.h"
#include "uart.h"
#include "util.h"
#include "vfs.h"

void main()
{
    uart_init();
    init_memory();

    uart_puts("r1/2 test\r\n");
    filesystem_t fs = tmpfs_filesystem();
    register_filesystem(&fs);

    file_t* a = vfs_open("hello", O_OPEN);
    if ((int)a != 0) {
        uart_puts("test 1 fail\r\n");
    }

    a = vfs_open("hello", O_CREAT);
    if ((int)a == 0) {
        uart_puts("test 2 fail\r\n");
    }

    vfs_close(a);
    file_t* b = vfs_open("hello", O_OPEN);

    uart_puts("================================\r\n");
    uart_puts("r3 test\r\n");
    char buf[32];
    a = vfs_open("hello", O_OPEN);
    b = vfs_open("world", O_CREAT);
    b = vfs_open("world", O_OPEN);

    vfs_write(a, "Hello ", 6);
    vfs_write(b, "World!", 6);
    vfs_close(a);
    vfs_close(b);
    b = vfs_open("hello", O_OPEN);
    a = vfs_open("world", O_OPEN);

    int sz;
    sz = vfs_read(b, buf, 100);
    sz += vfs_read(a, buf + sz, 100);
    buf[sz] = '\0';
    uart_puts(buf);
}
