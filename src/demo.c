#include "schedule.h"
#include "uart0.h"
#include "sys.h"

void delay(int period) {
    while (period--);
}

// Lab4: Required 1, 2

void demo_task_1() {
    while (1) {
        uart_printf("%d...\n", get_current_task()->id);
        delay(100000000);
    }
}

void demo_task_2() {
    while (1) {
        uart_printf("%d...\n", get_current_task()->id);
        delay(100000000);
    }
}

// Lab4: Required 3

void demo_do_exec_el0() {
    while(1) {
        uart_printf("hello from demo_do_exec_el0\n");
        delay(100000000);
    }
}

void demo_do_exec() {
    do_exec(demo_do_exec_el0);
}

// Lab4: Required 4

void demo_syscall_get_task_id() {
    while(1) {
        uart_printf("%d\n", get_taskid());
        delay(100000000);
    }
}

void demo_syscall_uart() {
    while(1) {
        char buf[256];
        uart_read(buf, 1);
        uart_printf("%d: ", get_taskid());
        uart_write(buf, 1);
        uart_printf("\n");
    }
}

void demo_syscall() {
    do_exec(demo_syscall_uart);
}
