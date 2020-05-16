#include "schedule.h"
#include "uart0.h"
#include "sys.h"

void delay(int period) {
    while (period--);
}

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

void demo_do_exec_el0() {
    while(1) {
        uart_printf("hello from demo_do_exec_el0\n");
        delay(100000000);
    }
}

void demo_do_exec() {
    do_exec(demo_do_exec_el0);
}

void demo_syscall() {
    while(1) {
        uart_printf("%d\n", get_taskid());
        delay(100000000);
    }
}
