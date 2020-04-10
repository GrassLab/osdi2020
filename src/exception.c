#include "../include/uart.h"

void debug(){
    uart_puts("debugggggg!!\n");
}
/**
 * common exception handler
 */
void exc_handler(unsigned long type)
{
    unsigned long esr; 
    asm volatile ("mrs %0, esr_el2" : "=r"(esr));
    unsigned long elr; 
    asm volatile ("mrs %0, elr_el2" : "=r"(elr));
    unsigned long spsr; 
    asm volatile ("mrs %0, spsr_el2" : "=r"(spsr));
    unsigned long far; 
    asm volatile ("mrs %0, far_el2" : "=r"(far));

    // print out interruption type
    switch(type) {
        case 0: uart_puts("Synchronous"); break;
        case 1: uart_puts("IRQ"); break;
        case 2: uart_puts("FIQ"); break;
        case 3: uart_puts("SError"); break;
    }
    uart_puts(": ");
    
    // decode exception type (some, not all. See ARM DDI0487B_b chapter D10.2.28)
    switch(esr>>26) { 
        case 0b000000: uart_puts("Unknown"); break;
        case 0b000001: uart_puts("Trapped WFI/WFE"); break;
        case 0b001110: uart_puts("Illegal execution"); break;
        case 0b010101: uart_puts("System call"); break;
        case 0b100000: uart_puts("Instruction abort, lower EL"); break;
        case 0b100001: uart_puts("Instruction abort, same EL"); break;
        case 0b100010: uart_puts("Instruction alignment fault"); break;
        case 0b100100: uart_puts("Data abort, lower EL"); break;
        case 0b100101: uart_puts("Data abort, same EL"); break;
        case 0b100110: uart_puts("Stack alignment fault"); break;
        case 0b101100: uart_puts("Floating point"); break;
        case 0b110000: uart_puts("Breakpoint, lower EL"); break;
        case 0b110001: uart_puts("Breakpoint, same EL"); break;
        case 0b111100: uart_puts("Breakpoint instruction"); break;
        default: uart_puts("Unknown"); break;
    }
    
    // print EC and ISS
    uart_puts("\n\t Exception Class (EC): ");
    uart_hex(esr>>26);
    uart_puts("\n\t Instr Specific Syndrome (ISS): ");
    uart_hex(esr&0x1ffffff);

    // dump registers
    uart_puts("\n\t  ESR_ELx: 0x");
    uart_hex(esr);
    
    uart_puts("\n\t  ELR_ELx: 0x");
    uart_hex(elr);

    uart_puts("\n\t SPSR_ELx: 0x");
    uart_hex(spsr);
    
    uart_puts("\n\t  FAR_ELx: 0x");
    uart_hex(far);
    uart_puts("\n");

    // breakpoint jump out
    if (esr>>26==0b110000 || esr>>26==0b110001 || esr>>26==0b111100) {
        asm volatile ("mrs %0, elr_el2" : "=r"(elr));
        asm volatile ("msr elr_el2, %0" : : "r" (elr+4));
    }  
}

