#include <stdint.h>

#include "io.h"
#include "lfb.h"
#include "mbox.h"
#include "reset.h"
#include "shell.h"
#include "syscall.h"
#include "task.h"
#include "uart.h"

#define GET_BOARD_REVISION 0x00010002
#define REQUEST_CODE 0x00000000
#define REQUEST_SUCCEED 0x80000000
#define REQUEST_FAILED 0x80000001
#define TAG_REQUEST_CODE 0x00000000
#define END_TAG 0x00000000

#define GET_VC_MEMORY 0x00010006
#define UART_ID 0x000000002

extern volatile unsigned int _boot_start;
extern volatile unsigned int _end;
extern volatile void set_aux();

void run() {
    print_s("\033[2J\033[1;1H");
    print_s(
        "██████╗  ██████╗  ██████╗ ████████╗██╗      ██████╗  █████╗ ██████╗ "
        "███████╗██████╗ \r\n"
        "██╔══██╗██╔═══██╗██╔═══██╗╚══██╔══╝██║     "
        "██╔═══██╗██╔══██╗██╔══██╗██╔════╝██╔══██╗\r\n"
        "██████╔╝██║   ██║██║   ██║   ██║   ██║     ██║   ██║███████║██║  "
        "██║█████╗  ██████╔╝\r\n"
        "██╔══██╗██║   ██║██║   ██║   ██║   ██║     ██║   ██║██╔══██║██║  "
        "██║██╔══╝  ██╔══██╗\r\n"
        "██████╔╝╚██████╔╝╚██████╔╝   ██║   ███████╗╚██████╔╝██║  "
        "██║██████╔╝███████╗██║  ██║\r\n"
        "╚═════╝  ╚═════╝  ╚═════╝    ╚═╝   ╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚═════╝ "
        "╚══════╝╚═╝  ╚═╝\r\n");
    while (1) {
        shell();
    }
}

void idle() {
    if (runqueue.head->next == 0) {
        print_s("No task");
        while (1)
            ;
    } else {
        schedule();
    }
}

void user1_1() {
    uart_write("user 0\n", 7);
    while (1)
        ;
}

void user1() {
    uart_write("user 1\n", 7);
    exec(user1_1);
    while (1)
        ;
}

void user2() {
    print_s("user2\n");
    int a = fork();
    if (a == 0) {
        print_s("pid: ");
        print_i(a);
        print_s("\n");
        while (1)
            ;
    } else {
        print_s("pid: ");
        print_i(a);
        print_s("\n");
        exit(2);
        print_s("Should not be printed\n");
    }
}

void user3() {
    uart_write("user 3\n", 7);
    kill(2, SIGKILL);
    while (1)
        ;
}

void user4() {
    uart_write("user 4\n", 7);
    char ch[10];
    uart_read(ch, 1);
    print_s("your input: ");
    uart_write(ch, 1);
    print_s("\n");
    while (1)
        ;
}

void task1() {
    uart_write("task 1\n", 7);
    /* do_exec(user1); */
    kexit(3);
}

void task2() {
    uart_write("task 2\n", 7);
    do_exec(user2);
}

void task3() {
    uart_write("task 3\n", 7);
    do_exec(user3);
}

void task4() {
    uart_write("task 4\n", 7);
    /* while (1) */
    /* ; */
    do_exec(user4);
}

void zombie_killer() {
    while (1) {
        for (int i = 0; i < 64; i++) {
            if (task_pool[i].status == ZOMBIE) {
                task_pool[i].status = INACTIVE;
                break;
            }
        }
    }
}

int main() {
    // set up serial console
    uart_init();
    lfb_init();
    lfb_showpicture();
    task_init();
    set_aux();
    /* char tmp = uart_getb(); */
    /* tmp = uart_getb(); */
    /* tmp = uart_getb(); */
    asm volatile("svc #2");

    privilege_task_create(idle, 3);
    /* privilege_task_create(zombie_killer, 0); */
    /* privilege_task_create(task1, 0); */
    /* privilege_task_create(task2, 0); */
    /* privilege_task_create(task3, 0); */
    privilege_task_create(task4, 0);
    privilege_task_run();

    /* run(); */
}
