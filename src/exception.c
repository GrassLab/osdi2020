#include "uart.h"
#include "string.h"
#include "printf.h"
#include "time.h"
#include "exception.h"

extern void core_timer_handler();

void handle_unknown() {
    printf("Unkown exception\r\n");
}

void irq_handler() {
    unsigned int irq = mm_read(CORE0_INT_SOURCE);
    if (irq) {
        if (irq == 0x800)
            local_timer_handler();
        else if (irq == 0x2)
            core_timer_handler();
        else
            printf("Unknown IRQ: %x\r\n", irq);
    }
}

void exception_handler(unsigned short level, unsigned long esr, unsigned long elr) {
    char exception_details[128];

    switch(esr >> 26) {
        case 0b000000: strcpy(exception_details, "Unknown"); break;
        case 0b000001: strcpy(exception_details, "Trapped WFI/WFE"); break;
        case 0b001110: strcpy(exception_details, "Illegal execution"); break;
        case 0b010101: strcpy(exception_details, "System call"); break;
        case 0b100000: strcpy(exception_details, "Instruction abort, lower EL"); break;
        case 0b100001: strcpy(exception_details, "Instruction abort, same EL"); break;
        case 0b100010: strcpy(exception_details, "Instruction alignment fault"); break;
        case 0b100100: strcpy(exception_details, "Data abort, lower EL"); break;
        case 0b100101: strcpy(exception_details, "Data abort, same EL"); break;
        case 0b100110: strcpy(exception_details, "Stack alignment fault"); break;
        case 0b101100: strcpy(exception_details, "Floating point"); break;
        case 0b111100: strcpy(exception_details, "Breakpoint"); break;
        default: strcpy(exception_details, "Unknown");
    }

    printf("Exception from EL%d\n", level);
    printf("Return address : 0x%X\n", elr);
    printf("Exception class : 0x%02hX (%s)\n", esr >> 26, exception_details);
    printf("Instruction specific syndrome : 0x%01hX\n", esr & 0xF);
    //while(1);
}
