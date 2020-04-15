#include "uart.h"

// Table is from:
// https://github.com/s-matyukevich/raspberry-pi-os/blob/master/src/lesson03/src/irq.c
const char *entry_error_messages[] = {
    "SYNC_INVALID_EL2t",
    "IRQ_INVALID_EL2t",     
    "FIQ_INVALID_EL2t",     
    "ERROR_INVALID_EL2T",       

    "SYNC_INVALID_EL2h",        
    "IRQ_INVALID_EL2h",     
    "FIQ_INVALID_EL2h",     
    "ERROR_INVALID_EL2h",       

    "SYNC_INVALID_EL0_64",      
    "IRQ_INVALID_EL0_64",       
    "FIQ_INVALID_EL0_64",       
    "ERROR_INVALID_EL0_64", 

    "SYNC_INVALID_EL0_32",      
    "IRQ_INVALID_EL0_32",       
    "FIQ_INVALID_EL0_32",       
    "ERROR_INVALID_EL0_32",
    "NotImplementedException"
};

void report_exception(int type, unsigned int esr, unsigned int address) {
    uart_puts("Exception encountered: ");
    uart_puts(entry_error_messages[type]);
    uart_puts("\nAddress: ");
    uart_hex(address);
    uart_puts("\nEC: ");
    uart_hex(esr >> 26);
    uart_puts("\nISS: ");
    uart_hex(esr & 0x1ffffff);
    uart_puts("\n");
}

int counter = 0;

void count() {
    uart_puts("counter: ");
    uart_hex(counter);
    uart_puts("\n");
    counter += 1;
}

int handle_sync_excpetion_el0_64(unsigned long esr, unsigned long address) {
    unsigned long iss = (esr & 0x1ffffff);
    uart_hex(iss);
    if (iss == 2) {
        core_timer_enable();
        return 0;
    }
    return -1;
} 
