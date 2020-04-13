#include "irq.h"
#include "timer.h"

void exception_handler()
{
    unsigned int esr, elr, spsr, far;
    unsigned int ec, iss, retaddr;
    asm volatile ("mrs %0, esr_el1" : "=r" (esr));
    asm volatile ("mrs %0, elr_el1" : "=r" (elr));
    asm volatile ("mrs %0, spsr_el1" : "=r" (spsr));
    asm volatile ("mrs %0, far_el1" : "=r" (far));
    ec = esr >> (32-6);
    iss = esr & (0xffffff);
    retaddr = elr >> 32;

    if (iss == SYSCALL_ENABLE_IRQ) {
        __enable_irq();
    } else if (iss == SYSCALL_CORE_TIMER_INIT) {
        __core_timer_init();
    } else if (iss == SYSCALL_LOCAL_TIMER_INIT) {
        __local_timer_init();
    }

    uart_puts("[info] Exception Class: 0x");
    uart_print_hex(ec);
    uart_puts("\n[info] Instruction Specific syndrome: 0x");
    uart_print_hex(iss);
    uart_puts("\n[info] Exception return address: 0x");
    uart_print_hex(retaddr);
    uart_puts("\n");
}