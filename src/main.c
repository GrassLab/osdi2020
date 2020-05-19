#include <stdint.h>

#include "io.h"
#include "lfb.h"
#include "mbox.h"
#include "page.h"
#include "reset.h"
#include "shell.h"
#include "syscall.h"
#include "task.h"
#include "tests.h"
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

void zombie_killer() {
    print_s("this is killer\n");
    while (1) {
        for (int i = 0; i < 64; i++) {
            if (task_pool[i].status == ZOMBIE) {
                task_pool[i].status = INACTIVE;
                break;
            }
        }
    }
}

void req1() {
    privilege_task_create(sche_task, 0);
    privilege_task_create(sche_task, 0);
}

void req3() {
    privilege_task_create(loop_task, 0);
    privilege_task_create(loop_task, 0);
}

void req4() {
    privilege_task_create(exec_task, 0);
    privilege_task_create(fork_exit_task, 0);
}

void ele1() {
    privilege_task_create(loop_user, 0);
    privilege_task_create(kill_task, 0);
}

void ele2() {
    privilege_task_create(kexit_task, 0);
    privilege_task_create(kexit_task, 0);
}

void ele3() { privilege_task_create(echo_task, 0); }

void ele5() {
    privilege_task_create(loop_ktask, 0);
    privilege_task_create(loop_ktask, 0);
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

    struct page_t* page = page_alloc();
    page_free(page);

    privilege_task_create(idle, 3);
    /* privilege_task_create(zombie_killer, 0); */
    /* privilege_task_create(task1, 0); */
    /* privilege_task_create(task2, 0); */
    /* privilege_task_create(task3, 0); */

    /* req1(); */
    req3();
    /* req4(); */
    /* ele1(); */
    /* ele2(); */
    /* ele3(); */
    /* ele5(); */
    privilege_task_run();

    /* run(); */
}
