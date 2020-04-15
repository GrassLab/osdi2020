#include "io.h"
#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define EXPIRE_PERIOD 0xffff
extern void disable_irq();
extern void enable_irq();

int time_c = 0;
void core_timer_enable() {
    asm volatile(
        "mov x1, x0;"
        "mov x0, 1;"
        "msr cntp_ctl_el0, x0;"  // enable timer
        "mov x0, 0xfffff;"
        "msr cntp_tval_el0, x0;"  // set expired time
        "mov x0, 2;"
        "ldr x1, =0x40000040;"
        "str x0, [x1];"  // enable timer interrupt
        "msr daifclr, #2;");
}

void core_timer_handler() {
    time_c++;
    print_s("System timer interrupt ");
    print_i(time_c);
    print_s("\n");

    asm volatile("mov x0, 0x1");
    asm volatile("mrs x1, CNTFRQ_EL0");
    asm volatile("mul x0, x0, x1");
    asm volatile("msr cntp_tval_el0, x0");
    return;
}
