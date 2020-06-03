#include <stdint.h>

#include "alloc.h"
#include "io.h"
#include "page.h"
#include "pool.h"
#include "syscall.h"
#include "task.h"
#include "tests.h"
#include "uart.h"

extern volatile unsigned int _boot_start;
extern uint64_t _end;
extern uint64_t _pte_base;
extern void set_aux();
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
    uint8_t* p8 = kmalloc(sizeof(uint8_t));
    kfree(p8);
    uint8_t* p8_2 = kmalloc(sizeof(uint8_t));
    kfree(p8_2);

    uint32_t* p32 = kmalloc(sizeof(uint32_t) * 1024);
    kfree(p32);
    uint32_t* p32_2 = kmalloc(sizeof(uint32_t) * 1024);
    kfree(p32_2);
    /* do_exec((uint8_t*)&_binary_user_img_start, */
    /* ((uint64_t)(&_binary_user_img_end) - */
    /* (uint64_t)(&_binary_user_img_start))); */
    while (1)
        ;
}

void foo() { privilege_task_create(foo_kernel, 0); }

int main() {
    // set up serial console
    uart_init();
    task_init();
    set_aux();
    buddy_init();
    // init core timer
    asm volatile("svc #2");

    privilege_task_create(idle, 3);
    foo();
    privilege_task_run();
}
