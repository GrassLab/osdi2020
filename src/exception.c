#include "../include/uart.h"

// array to store register values 
unsigned long dbg_regs[37];

/**
 * common exception handler
 */
void exc_handler(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far)
{
    // uart_puts("CurrentEL: ");
    // uart_hex(level);
    // uart_puts("\n");

    // print out interruption type
    switch(type) {
        case 0: uart_puts("Synchronous"); break;
        case 1: uart_puts("IRQ"); break;
        case 2: uart_puts("FIQ"); break;
        case 3: uart_puts("SError"); break;
    }
    
    // print EC and ISS
    uart_puts(": ");
    uart_puts("\n Exception Class (EC): ");
    uart_hex(esr>>26);
    uart_puts("\n Instr Specific Syndrome (ISS): ");
    uart_hex(esr&0xffffff);
    uart_puts("\n");
    
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
    
    // dump registers
    uart_puts(":\n  ESR_EL2 ");
    uart_hex(esr>>32);
    uart_hex(esr);
    uart_puts(" ELR_EL2 "); // exception return address
    uart_hex(elr>>32);
    uart_hex(elr);

   

    uart_puts("\n SPSR_EL2 ");
    uart_hex(spsr>>32);
    uart_hex(spsr);
    uart_puts(" FAR_EL2 ");
    uart_hex(far>>32);
    uart_hex(far);
    uart_puts("\n");
    // no return from exception for now
    // exception return should be implement at start.S
    //while(1);

    
    // uart_puts("Exception in debugger!\n")
    // uart_puts("  elr_el1: ");
    // uart_hex(dbg_regs[31]);
    // uart_puts("  spsr_el1: ");
    // uart_hex(dbg_regs[32]);
    // uart_puts("\n");

    // uart_puts("  esr_el1: ");
    // uart_hex(dbg_regs[33]);
    // uart_puts("  far_el1: ");
    // uart_hex(dbg_regs[34]);
    // uart_puts("\n");

    // uart_puts("  sctlr_el1: ");
    // uart_hex(dbg_regs[35]);
    // uart_puts("  tcr_el1: ");
    // uart_hex(dbg_regs[36]);
    // uart_puts("\n");
    
}

