#include "io.h"
#include "timer.h"

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
    }
}
