#include "uart.h"
#include "string.h"
#include "printf.h"
#include "time.h"
#include "exception.h"

extern void core_timer_handler();

extern void enable_irq();

extern void disable_irq();

extern void init_irq();

void handle_unknown() {
    unsigned int el;
    asm volatile("mrs %0, CurrentEL; lsr %0, %0, 2" : "=r"(el));
    printf("Unkown exception from EL%d \r\n", el);
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

void exception_handler(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long x4, unsigned long x5, unsigned long x8) {
    unsigned long esr, elr;
    unsigned int el, ec, iss;
    char exception_details[128];

    // get the exception level to use the appropriate registers
    asm volatile("mrs %0, CurrentEL; lsr %0, %0, 2" : "=r"(el));

    if (el == 0 || el == 1) {
        asm volatile("mrs %0, ELR_EL1" : "=r"(elr));
        asm volatile("mrs %0, ESR_EL1" : "=r"(esr));
    } else if (el == 2) {
        asm volatile("mrs %0, ELR_EL2" : "=r"(elr));
        asm volatile("mrs %0, ESR_EL2" : "=r"(esr));    
    } else {
        printf("Wrong exception level, aborting\r\n");
        while(1);
    }

    ec = esr >> 26;
    iss = esr & 0xF;

    if (ec == 0b010101 && iss == 0) { // handle syscall
        syscall(x0, x1, x2, x3, x4, x5, x8); 
    } else {
        switch(ec) {
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

        printf("Exception issued in EL%d\n", el);
        printf("Return address : 0x%X\n", elr);
        printf("Exception class : 0x%02hX\n", ec);
        printf("Instruction specific syndrome : 0x%01hX\n", iss);
    }
}


// (xO, x1), (x2, x3), (x4, x5) used for parameters, x8 for the syscall id
// source: linux kernel tree
void syscall(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long x4, unsigned long x5, unsigned long x8) {
    switch(x0) {
        case 1:
            get_timestamp((float *) x1); 
            break;
        case 0:
            //enable_irq();
            core_timer_enable();
            //disable_irq();
            break;
        default:
            printf("Unknown syscall\r\n");
            break;
    }
}
