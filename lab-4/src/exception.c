#include "irq.h"
#include "timer.h"
#include "utility.h"

void exception_handler()
{
    unsigned int el_level;
    unsigned int esr, elr, spsr, far;
    unsigned int ec, iss, retaddr;
    asm volatile ("mrs %0, CurrentEL" : "=r" (el_level));
    if (el_level == 0x4) {
        asm volatile ("mrs %0, esr_el1" : "=r" (esr));
        asm volatile ("mrs %0, elr_el1" : "=r" (elr));
        asm volatile ("mrs %0, spsr_el1" : "=r" (spsr));
        asm volatile ("mrs %0, far_el1" : "=r" (far));
    } else if (el_level == 0x8) {
        asm volatile ("mrs %0, esr_el2" : "=r" (esr));
        asm volatile ("mrs %0, elr_el2" : "=r" (elr));
        asm volatile ("mrs %0, spsr_el2" : "=r" (spsr));
        asm volatile ("mrs %0, far_el2" : "=r" (far));
    }
    ec = esr >> (32-6);
    iss = esr & (0xffffff);
    retaddr = elr;

    if (iss == SYSCALL_ENABLE_IRQ) {
        __enable_irq();
    } else if (iss == SYSCALL_CORE_TIMER_INIT) {
        __core_timer_init();
    } else if (iss == SYSCALL_LOCAL_TIMER_INIT) {
        __local_timer_init();
    } else if (iss == GET_TIMESTAMP) {
        __getTimestamp();
    }

    uart_puts("[info] Exception Level: 0x");
    uart_print_hex(el_level);
    uart_puts("\n[info] Exception Class: 0x");
    uart_print_hex(ec);
    uart_puts("\n[info] Instruction Specific syndrome: 0x");
    uart_print_hex(iss);
    uart_puts("\n[info] Exception return address: 0x");
    uart_print_hex(retaddr);
    uart_puts("\n\n");
}