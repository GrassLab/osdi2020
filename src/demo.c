#include "schedule.h"
#include "uart0.h"

void delay(int period) {
    while (period--);
}

void demo_task_1() {
    while (1) {
        uart_printf("%d...\n", get_current_task()->id);
        delay(100000000);
        context_switch(&task_pool[2]);
    }
}

void demo_task_2() {
    while (1) {
        uart_printf("%d...\n", get_current_task()->id);
        delay(100000000);
        context_switch(&task_pool[1]);
    }
}