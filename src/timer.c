#include "io.h"
#include "reset.h"
#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define EXPIRE_PERIOD 0xffff
#define LOCAL_TIMER_CONTROL 0x40000034

extern void disable_irq();
extern void enable_irq();

int system_time_c = 0;
int local_time_c = 0;
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
    system_time_c++;
    print_s("System timer interrupt ");
    print_i(system_time_c);
    print_s("\n");

    asm volatile("mov x0, 0x1");
    asm volatile("mrs x1, CNTFRQ_EL0");
    asm volatile("mul x0, x0, x1");
    asm volatile("msr cntp_tval_el0, x0");
    return;
}

#define LOCAL_TIMER_CONTROL_REG 0x40000034

void local_timer_init() {
    unsigned int flag = 0x30000000;  // enable timer and interrupt.
    unsigned int reload = 25000000;
    set(LOCAL_TIMER_CONTROL, flag | reload);
    asm volatile("msr daifclr, #2;");
}

#define LOCAL_TIMER_IRQ_CLR 0x40000038

void local_timer_handler() {
    local_time_c++;
    print_s("Local timer interrupt ");
    print_i(local_time_c);
    print_s("\n");
    set(LOCAL_TIMER_IRQ_CLR, 0xc0000000);  // clear interrupt and reload.
    int k = 214143141;
    asm volatile("msr daifclr, #2;");
    while (k--) {
        asm volatile("nop");
    }
}
