#include "uart.h"

/**
 * common exception handler
 */
void exc_handler(unsigned long type, unsigned long esr, unsigned long elr,
                 unsigned long spsr, unsigned long far) {
    switch (type) {
        case 0:
            print("Synchronous");
            break;
        case 1:
            print("IRQ");
            break;
        case 2:
            print("FIQ");
            break;
        case 3:
            print("SError");
            break;
    }
    print("Exception return address: %x", elr);
    print("Exception class (EC): %x", elr >> 26);
    print("Instruction specific syndrome (ISS): %x", esr & ((1 << 24) - 1));
}
