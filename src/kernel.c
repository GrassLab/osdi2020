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

int kernel_main()
{
    uart_init();
    init_printf(0, uart_putc);  
    get_board_info();
    mm_init();
    rootfs_init();
    printf("ok");
    // test1();

    core_timer_enable();
    idle_task();
    return -1;
}
