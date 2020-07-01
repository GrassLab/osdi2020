#include "exce.h"
#include "mailbox.h"
#include "mm.h"
#include "printf.h"
#include "schedule.h"
#include "sdcard.h"
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
    struct file *c, *b, *a = vfs_open("hello", 0);
    assert(a == 0, "0");
    a = vfs_open("/BOOTCODE.BIN", 0);
    assert(a != 0, "1");
    vfs_close(a);
    b = vfs_open("/dir/hello", 0);
    assert(b == 0, "2");
    b = vfs_open("/hello", O_CREAT);
    assert(b != 0, "3");
    vfs_close(b);
    c = vfs_open("/hello", O_CREAT);// should open the same file
    assert(c != 0, "3-1");
    vfs_close(c);
    b = vfs_open("/hello", 0);
    assert(b != 0, "4");
    vfs_close(b);
}

void test2()
{
    char buf[100];
    struct file* a = vfs_open("/hello", O_CREAT);
    struct file* b = vfs_open("/world", O_CREAT);
    vfs_write(a, "Hello ", 6);
    vfs_write(b, "World!", 6);
    vfs_close(a);
    vfs_close(b);
    b = vfs_open("/hello", 0);
    a = vfs_open("/world", 0);
    int sz;
    sz = vfs_read(b, buf, 100);
    sz += vfs_read(a, buf + sz, 100-sz);
    buf[sz] = '\0';
    printf("%s\n", buf); // should be Hello World!

    vfs_ls("/");
}

void sdcard_test(){ 
    sd_init();
    char buf[512]={0};
    readblock(0, buf);
    for (int i=0; i<512/4;i++){
        if(i%4 == 0)
            printf("\n");
        printf("0x%08x\t", ((long *)buf)[i]);
    }
}

int kernel_main()
{
    uart_init();
    init_printf(0, uart_putc);  
    get_board_info();
    mm_init();
    rootfs_init();
    // sdcard_test();
    printf("\n\n\ntest1\n");
    test1();
    printf("\n\n\ntest2\n");
    test2();

    core_timer_enable();
    idle_task();
    return -1;
}
