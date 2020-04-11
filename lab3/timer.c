#include "uart.h"
#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define EXPIRE_PERIOD 0xfffffff
void core_timer_enable(){   
    uart_puts("irq\n");
    asm volatile ("mov x0, 1;"
                  "msr cntp_ctl_el0, x0 // enable timer;"
                  "mov x0, 2;"
                  "ldr x1, =0x40000040;"
                  "str x0, [x1] // enable timer interrupt");
}