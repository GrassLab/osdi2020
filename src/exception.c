#include "../include/uart.h"
#include "../include/info.h"
#include "../include/interrupt.h"

void exception_not_implement()
{
    uart_puts("Exception not implement!\n"); 
    while (1);
}

/**
 * common exception handler
 */
// follow aarch64 calling convention in system call
// syscall_x0 (x1, x2, x3)
void exception_handler(unsigned int x0, unsigned int x1, unsigned int x2, unsigned int x3)
{
    unsigned long esr, elr, spsr; 

    //  check exception level
    int level = get_exception_level();
    switch(level) {
        case 1: 
            asm volatile ("mrs %0, esr_el1" : "=r"(esr));
            asm volatile ("mrs %0, elr_el1" : "=r"(elr));
            asm volatile ("mrs %0, spsr_el1" : "=r"(spsr));
            break;
        case 2: 
            asm volatile ("mrs %0, esr_el2" : "=r"(esr));
            asm volatile ("mrs %0, elr_el2" : "=r"(elr));
            asm volatile ("mrs %0, spsr_el2" : "=r"(spsr));
            break;
        default: 
            uart_puts("Unknown Exception level\n"); 
            return;
    }    

    if ((esr>>26==0b010101) && ((esr&0x1ffffff)==0)) {
        switch (x0) {
            case 0:
                uart_puts("syscall core timer enable.\n");
                core_timer_enable(); break;
        }
    } 
    else {
        decode_exception(esr, elr, spsr);
    }

    // breakpoint jump out
    if (esr>>26==0b110000 || esr>>26==0b110001 || esr>>26==0b111100) {
        switch(level) {
            case 1:
                asm volatile ("msr elr_el1, %0" : : "r" (elr+4)); break;
            case 2:
                asm volatile ("msr elr_el2, %0" : : "r" (elr+4)); break;
            case 3:
                asm volatile ("msr elr_el3, %0" : : "r" (elr+4)); break;  
        }  
    }
}

void decode_exception(unsigned long esr, unsigned long elr, unsigned long spsr)
{
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
    uart_hex(esr>>16);
    uart_hex(esr);
    
    uart_puts("\n\t  ELR_ELx: 0x");
    uart_hex(elr>>16);
    uart_hex(elr);

    uart_puts("\n\t SPSR_ELx: 0x");
    uart_hex(spsr>>16);
    uart_hex(spsr);
    uart_puts("\n")
}

