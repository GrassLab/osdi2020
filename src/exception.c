#include "uart0.h"
#include "peripherals/timer.h"

void init_irq() {
    // Enable IMO
    register unsigned int hcr_el2_value;
    asm volatile ("mrs %0, hcr_el2" : "=r" (hcr_el2_value));
    hcr_el2_value |= 1 << 4; // IMO
    asm volatile ("msr hcr_el2, %0" : : "r" (hcr_el2_value));
    // Unmask Interrupt
    register unsigned int unmask = 0;
    asm volatile ("msr daif, %0" : : "r" (unmask));
}

void arm_core_timer_enable() {
    // enable timer
    register unsigned int enable = 1;
    asm volatile ("msr cntp_ctl_el0, %0" : : "r" (enable));
    // set expired time
    register unsigned int expire_period = 0xfffffff;
    asm volatile ("msr cntp_tval_el0, %0" : : "r" (expire_period));
    // enable timer interrupt
    *CORE0_TIMER_IRQ_CTRL = 2;
}

void sync_el2h_router(unsigned long esr, unsigned long elr) {
    uart_printf("Exception return address 0x%x\n", elr);
    uart_printf("Exception class (EC) 0x%x\n", (esr >> 26) & 0b111111);
    uart_printf("Instruction specific syndrome (ISS) 0x%x\n", esr & 0xFFFFFF);
}
