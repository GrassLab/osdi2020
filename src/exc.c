#include "gpio.h"
#include "io.h"
#include "reset.h"
#include "timer.h"
#include "uart.h"

#define IRQ_BASIC_PENDING (volatile unsigned int *)(MMIO_BASE + 0xb204)
#define CORE0_IRQ_SOURCE (volatile unsigned int *)0x40000060

#define ENABLE_IRQS_1 ((volatile unsigned int *)(MMIO_BASE + 0x0000B210))
#define AUX_IRQ (1 << 29)
#define IIR_REG_REC_NON_EMPTY (2 << 1)

int buffer_now = 0;
int buffer_read = 0;

extern void core_timer_enable();
/**
 * common exception handler
 */
void exception_handler(unsigned long type, unsigned long esr,
                       unsigned long elr) {
    int iss = esr & ((1 << 24) - 1);
    int ec = elr >> 26;
    switch (type) {
        case 0:
            print_s("Synchronous\n");
            break;
        case 1:
            print_s("IRQ\n");
            break;
        case 2:
            print_s("FIQ\n");
            break;
        case 3:
            print_s("SError\n");
            break;
    }
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
        asm volatile("ti:");
        core_timer_enable();
        local_timer_init();
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
    if ((*IRQ_BASIC_PENDING) & AUX_IRQ) {
        if (*AUX_MU_LSR & 0x01) {
            buffer[buffer_now++] = (char)(*AUX_MU_IO);
        }
    } else if ((*CORE0_IRQ_SOURCE) & (1 << 11)) {
        local_timer_handler();
    } else if ((*CORE0_IRQ_SOURCE) & (1 << 1)) {
        core_timer_handler();
    }
}
