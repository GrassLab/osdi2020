#include "peripherals/uart.h"
#include "peripherals/irq.h"
#include "timer.h"
#include "sched.h"

void handle_irq() {
    // TODO: doc of CORE0_INTERRUPT_SOURCE
    unsigned int core0_irq_src = get32(CORE0_INTERRUPT_SOURCE);
    if (core0_irq_src == 2) {
        one_tick();
    }
}

void one_tick() {

    asm volatile ("msr cntp_tval_el0, %0" :: "r" (EXPIRE_PERIOD));
    current->counter --;
    
    if (current->counter <= 0 && current->preempt_count == 0) {
        current->counter = 0;
        enable_irq();
        schedule();
        disable_irq();
    }
}
