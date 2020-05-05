#include <stdint.h>

#include "gpio.h"
#include "io.h"
#include "reset.h"
#include "task.h"
#include "timer.h"
#include "uart.h"

#define IRQ_BASIC_PENDING (volatile unsigned int *)(MMIO_BASE + 0xb204)
#define CORE0_IRQ_SOURCE (volatile unsigned int *)0x40000060

#define ENABLE_IRQS_1 ((volatile unsigned int *)(MMIO_BASE + 0x0000B210))
#define AUX_IRQ (1 << 29)
#define IIR_REG_REC_NON_EMPTY (2 << 1)

extern void core_timer_enable();
void synchronize_handler() {
    uint64_t syscall, x0, x1, x2, x3, x4, x5;
    uint64_t esr, elr;
    asm volatile("mov %0, x0" : "=r"(x0));
    asm volatile("mov %0, x1" : "=r"(x1));
    asm volatile("mov %0, x2" : "=r"(x2));
    asm volatile("mov %0, x3" : "=r"(x3));
    asm volatile("mov %0, x4" : "=r"(x4));
    asm volatile("mov %0, x5" : "=r"(x5));
    asm volatile("mov %0, x8" : "=r"(syscall));
    asm volatile("mrs %0, esr_el1" : "=r"(esr));
    asm volatile("mrs %0, elr_el1" : "=r"(elr));
    int iss = esr & ((1 << 24) - 1);
    /* int ec = esr >> 26; */
    if (iss == 0) {
        switch (syscall) {
            case 0:
                uart_send(x0);
                break;
            case 1:
                x0 = uart_getc();
                asm volatile("mov x0, %0" : "=r"(x0));
                break;
            case 2:
                do_exec((void (*)())x0);
                break;
            case 3:
                break;
        }
    } else if (iss == 2) {
        core_timer_enable();
    }
}
/**
 * common exception handler
 */
void exception_handler() {
    uint64_t esr, elr;
    asm volatile("mrs %0, esr_el1" : "=r"(esr));
    asm volatile("mrs %0, elr_el1" : "=r"(elr));
    int iss = esr & ((1 << 24) - 1);
    int ec = esr >> 26;
    if (iss == 1) {
        print_s("Exception return address: 0x");
        print_h(elr);
        print_s("\n");
        print_s("Exception class (EC): 0x");
        print_h(ec);
        print_s("\n");
        print_s("Instruction specific syndrome (ISS): 0x");
        print_h(iss);
        print_s("\n");

    } else if (iss == 2) {
        core_timer_enable();
    } else {
        print_s("Exception return address: 0x");
        print_h(elr);
        print_s("\n");
        while (1)
            ;
    }
}

void set_aux() { *(ENABLE_IRQS_1) = AUX_IRQ; }

void irq_handler() {
    if ((*CORE0_IRQ_SOURCE) & (1 << 11)) {
        local_timer_handler();
    } else if ((*CORE0_IRQ_SOURCE) & (1 << 1)) {
        core_timer_handler();
    }
}
