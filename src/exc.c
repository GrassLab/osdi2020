#include "uart.h"

/**
 * common exception handler
 */
void exception_handler(unsigned long type, unsigned long esr, unsigned long elr,
                       unsigned long spsr, unsigned long far) {
    switch (type) {
        case 0:
            print("Synchronous\n");
            break;
        case 1:
            print("IRQ\n");
            break;
        case 2:
            print("FIQ\n");
            break;
        case 3:
            print("SError\n");
            break;
    }
    print("Exception return address: %x\n", elr);
    print("Exception class (EC): %x\n", elr >> 26);
    print("Instruction specific syndrome (ISS): %x\n", esr & ((1 << 24) - 1));
}
