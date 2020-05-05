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

void idle() { schedule(); }

void user0() {
    print_s("user0\n");
    while (1)
        ;
}

void user1() {
    /* uart_write('u'); */
    /* uart_write('s'); */
    /* uart_write('e'); */
    /* uart_write('r'); */
    /* uart_write(':'); */
    print_s("user1\n");
    exec(user0);
    while (1)
        ;
    /* char tmp; */
    /* while (1) { */
    /* tmp = uart_read(); */
    /* uart_write(tmp); */
    /* } */
}
void user2() {
    print_s("user2\n");
    fork();
    while (1)
        ;
}

void task1() {
    print_s("task 1\n");
    do_exec(user1);
}

void task2() {
    print_s("task 2\n");
    do_exec(user2);
}

void task3() {
    print_s("task 3\n");
    do_exec(user1);
}

int main() {
    // set up serial console
    uart_init();
    lfb_init();
    lfb_showpicture();
    task_init();
    asm volatile("svc #2");

    uint64_t spsr_el1;
    struct task_t* t1 = &task_pool[0];
    t1->elr = (uint32_t)idle;
    t1->id = 0;
    t1->used = 1;
    t1->time = 0;
    t1->utask.sp = (uint64_t)kstack_pool[1];
    asm volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
    t1->spsr = spsr_el1;
    t1->reschedule = 0;
    privilege_task_create(task1);
    privilege_task_create(task2);
    privilege_task_run(t1);

    /* run(); */
}
