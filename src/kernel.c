#include "exce.h"
#include "mailbox.h"
#include "mm.h"
#include "printf.h"
#include "schedule.h"
#include "svc.h"
#include "timer.h"
#include "vfs.h"
#include "uart.h"

void test1()
{
    struct file* a = vfs_open("hello", 0);
    // assert(a == NULL);
    printf("a");
    a = vfs_open("hello", O_CREAT);
    // assert(a != NULL);
    printf("a");
    vfs_close(a);
    struct file* b = vfs_open("hello", 0);
    // assert(b != NULL);
    printf("a");
    vfs_close(b);
}
void test2()
{
    char buf[100];
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
}

int kernel_main()
{
    uart_init();
    init_printf(0, uart_putc);  
    get_board_info();
    mm_init();
    rootfs_init();
    printf("ok");
    test1();
    test2();

    core_timer_enable();
    idle_task();
    return -1;
}
