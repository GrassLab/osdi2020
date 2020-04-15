#include "timer.h"

#define LOCAL_TIMER_CONTROL ((volatile unsigned int *)0x40000034)
#define LOCAL_TIMER_IRQ_CLR ((volatile unsigned int *)0x40000038)
#define LOCAL_TIMER_IRQ_CTRL ((volatile unsigned int *)0x40000040)

int core_timer_counter = 0;
int local_timer_counter = 0;

void core_timer_enable() {
    unsigned int a = 1, b = 0, c;

    asm volatile("mov x0, 1");
    asm volatile("msr cntp_ctl_el0, x0");
    asm volatile("mov x0, 2");
    asm volatile("ldr x1, =0x40000040");
    asm volatile("str x0, [x1]");
    asm volatile("mov x0, 0xffffff");
    asm volatile("msr cntp_tval_el0, x0");
    while (b > 0) {
    asm volatile ("mrs %0, cntp_ctl_el0" : "=r"(a));
    asm volatile ("mrs %0, cntp_tval_el0" : "=r"(b));
    asm volatile ("mrs %0, cntp_cval_el0" : "=r"(c));
    uart_puts("ctl_el0: ");
    uart_hex(a);
    uart_puts("\ntval_el0: ");
    uart_hex(b);
    uart_puts("\ncval_el0: ");
    uart_hex(c);
    uart_puts("\n");
    }
}

void core_timer_handler() {
    core_timer_counter++;
    uart_puts("Core timer interrupt, ");
    uart_int(core_timer_counter);
    uart_puts(" times\n");
    asm volatile("mov x0, 0xffffff");
    asm volatile("msr cntp_tval_el0, x0");
}


void local_timer_enable(){
    unsigned int flag = 0x30000000; // enable timer and interrupt.
    unsigned int reload = 25000000;
    *LOCAL_TIMER_CONTROL = flag | reload;
}

void local_timer_handler(){
    local_timer_counter++;
    uart_puts("Local timer interrupt, jiffies ");
    uart_int(local_timer_counter);
    uart_puts(" times\n");

    *LOCAL_TIMER_IRQ_CLR = 0xc0000000; // clear interrupt and reload.
}
