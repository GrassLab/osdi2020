#include "peripherals/uart.h"
#include "fork.h"

const unsigned int delay_interval = 100000000;

void foo() {
    while(1) {
        uart_puts("Task ID: ");
        uart_send_ulong(current -> task_id);
        uart_puts("; task_struct ptr: ");
        uart_send_hex(current);
        uart_puts("; sp: ");
        uart_send_hex(current -> cpu_context.sp);
        uart_send('\n');
        delay(delay_interval);
        schedule();
    }
}

void idle() {
    while(1){
        schedule();
        delay(delay_interval);
    }
}

void kernel_main() {
    uart_init();
    el1_vector_init();
    enable_core_timer();
    enable_irq();

    uart_puts("Welcome to MiniKernel 0.0.4\n");
    int ret;
    for (int i = 0; i < 5; ++ i) {
        ret = privilege_task_create((unsigned long)&foo, 0);
        if (ret != 0) {
            uart_puts("Error when creating privilege task ");
            uart_send_ulong(i);
            uart_send('\n');
        }
    }
    
    while (1) {
        delay(delay_interval);
    }
    // idle();
}
