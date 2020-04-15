#include "exception/irq.h"
#include "exception/timer.h"
#include "MiniUart.h"

void enable_irq(void) {
    /* close this for letting interrupt goto kernel in EL1 instead of EL2 */
    // enable physical IRQ routing
    // asm volatile("mrs x0, HCR_EL2");
    // asm volatile("orr x0, x0, #16"); // 0b10000, IMO @ HCR_EL2[4]
    // asm volatile("msr HCR_EL2, x0");

    asm volatile("msr daifclr, #0x2"); // clear I bit in DAIF
}

void irq_handler(void) {
    static int timer_counter = 1;

    _core_timer_handler();

    sendStringUART("Core timer interrupt counter: ");
    sendHexUART(timer_counter); 
    sendUART('\n');

    ++timer_counter;
}
