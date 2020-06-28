#include "exce.h"
#include "mailbox.h"
#include "mm.h"
#include "printf.h"
#include "schedule.h"
#include "svc.h"
#include "timer.h"
#include "uart.h"
#include "vfs.h"

void assert(int stament, const char *msg)
{
    if(stament)
        return;
    printf("Assertion False. Msg:%s\n", msg);
    do_exit(-1);
}

void test1()
{
    struct file *b, *a = vfs_open("hello", 0);
    assert(a == 0, "0");
    a = vfs_open("/hello", 0);
    assert(a == 0, "1");
    a = vfs_open("/dir/hello", 0);
    assert(a == 0, "2");
    a = vfs_open("/hello", O_CREAT);
    assert(a != 0, "3");
    b = vfs_open("/hello", O_CREAT);// should open the same file
    assert(b != 0, "3-1");
    vfs_close(a);
    vfs_close(b);
    char buf[10];
    for(int i = 1; i<NR_CHILD; i++){
        sprintf(buf, "/hello%2d",i);
        a = vfs_open(buf, O_CREAT);
        assert(a != 0, "3-2");
        vfs_close(a);
    }
    a = vfs_open("/hello16", O_CREAT);//only support limit entries
    assert(a == 0, "3-3");
    vfs_close(a);
    // b = vfs_open("hello", 0);
    // assert(b != 0, "4");
    // vfs_close(b);
}
// void test2()
// {
//     char buf[100];
//     struct file* a = vfs_open("hello", O_CREAT);
//     struct file* b = vfs_open("world", O_CREAT);
//     vfs_write(a, "Hello ", 6);
//     vfs_write(b, "World!", 6);
//     vfs_close(a);
//     vfs_close(b);
//     b = vfs_open("hello", 0);
//     a = vfs_open("world", 0);
//     int sz;
//     sz = vfs_read(b, buf, 100);
//     sz += vfs_read(a, buf + sz, 100);
//     buf[sz] = '\0';
//     printf("%s\n", buf); // should be Hello World!
// }

int kernel_main()
{
    uart_init();
    init_printf(0, uart_putc);  
    get_board_info();
    mm_init();
    rootfs_init();

    printf("\n\n\ntest1\n");
    test1();
    // printf("\n\n\ntest2\n");
    // test2();

    core_timer_enable();
    idle_task();
    return -1;
}
