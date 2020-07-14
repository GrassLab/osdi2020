#include "uart.h"
#include "mbox.h"
#include "shell.h"
#include "framebuffer.h"
#include "timer.h"
#include "irq.h"
#include "config.h"
#include "thread.h"
#include "syscall.h"
#include "sched.h"
#include "mm.h"
#include "vfs.h"

task_manager_t TaskManager;
task_t* current;

void init_uart(){
    uart_init();
    get_board_revision();
    get_vc_memory();
    get_UART_clock();
    // set_UART_clock();
    // get_UART_clock();
}

void init_lfb(){
    lfb_init();
    lfb_showpicture();
}

void test_vfs1()
{
    file_t* a = vfs_open("hello", 0);
    // assert(a == NULL);
    printf("a");
    a = vfs_open("hello", O_CREAT);
    // assert(a != NULL);
    printf("a");
    vfs_close(a);
    file_t* b = vfs_open("hello", 0);
    // assert(b != NULL);
    printf("a");
    vfs_close(b);
}
void test_vfs2()
{
    char buf[100];
    file_t* a = vfs_open("hello", O_CREAT);
    file_t* b = vfs_open("world", O_CREAT);
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
    printf("////output is %s\n", buf); // should be Hello World!
    vfs_list_file("/");
}

void main()
{
    init_uart();
    init_printf(0, putc);

    mem_alloc_init();
    rootfs_init();
    printf("rootfs init finish\n");
    test_vfs1();
    test_vfs2();
    // idle_task();
    while(1){
        printf("waittt\n");
        delay(100000000);
    }

    // echo everything back
    // while(1) {
    //     uart_send(uart_getc());
    // }

}
