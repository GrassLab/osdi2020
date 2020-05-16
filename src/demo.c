#include "schedule.h"
#include "uart0.h"

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