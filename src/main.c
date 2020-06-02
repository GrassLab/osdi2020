#include <stdint.h>

#include "alloc.h"
#include "io.h"
#include "page.h"
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
extern uint64_t _binary_user_img_start;
extern uint64_t _binary_user_img_end;

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

void foo_kernel() {
    print_s("foo kernel\n");
    do_exec((uint8_t *)&_binary_user_img_start,
            ((uint64_t)(&_binary_user_img_end) -
             (uint64_t)(&_binary_user_img_start)));
    while (1)
        ;
}

void foo() {
    privilege_task_create(foo_kernel, 0);
    privilege_task_create(foo_kernel, 0);
}

int main() {
    // set up serial console
    uart_init();
    task_init();
    set_aux();
    asm volatile("svc #2");

    /* privilege_task_create(idle, 3); */
    /* foo(); */
    /* privilege_task_run(); */
    buddy_init();
    print_s("buddy init done\n");
    int *a = kmalloc(4);
    *a = 3;
    print_s("kmalloc init done\n");
    asm volatile("a:");
    while (1)
        ;

    /* run(); */
}
