#include "io.h"
extern void disable_irq();
extern void enable_irq();

int time_c = 0;

void core_timer_handler() {
    time_c++;
    print_s("System timer interrupt ");
    print_i(time_c);
    print_s("\n");
    asm volatile("mov x0, 0xffffff");
    asm volatile("msr cntp_tval_el0, x0");
    return;
}
