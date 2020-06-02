#include "entry.h"
#include "peripherals/uart.h"

const char *entry_error_messages[] = { 
    "SYNC_INVALID_EL1t\n",
    "IRQ_INVALID_EL1t\n",    
    "FIQ_INVALID_EL1t\n",    
    "ERROR_INVALID_EL1T\n",    

    "SYNC_INVALID_EL1h\n",    
    "IRQ_INVALID_EL1h\n",    
    "FIQ_INVALID_EL1h\n",    
    "ERROR_INVALID_EL1h\n",    

    "SYNC_INVALID_EL0_64\n",    
    "IRQ_INVALID_EL0_64\n",    
    "FIQ_INVALID_EL0_64\n",    
    "ERROR_INVALID_EL0_64\n",   

    "SYNC_INVALID_EL0_32\n",    
    "IRQ_INVALID_EL0_32\n",    
    "FIQ_INVALID_EL0_32\n",    
    "ERROR_INVALID_EL0_32\n",

    "SYNC_ERROR\n",
    "SYSCALL_ERROR\n"

};

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address) {
    uart_puts("Error: ");
    uart_puts(entry_error_messages[type]);
    uart_puts("\n");
    uart_puts("EC: ");
    uart_send_hex(esr >> 26);
    uart_puts("; ISS: ");
    uart_send_hex(esr & 0x1ffffff);
}
