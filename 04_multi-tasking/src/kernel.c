#include "peripherals/uart.h"
#include "fork.h"

void print_task() {
    while (1) {
        uart_send('a');
        delay(10000000);
    }
}

void kernel_main() {
    uart_send_string("Welcome to MiniKernel 0.0.4");
    uart_send('\n');

    tid_t tid0 = privilege_task_create(print_task);    
}
