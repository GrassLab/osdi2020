#include "../include/entry.h"
#include "../include/mini_uart.h"

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
	"ERROR_INVALID_EL0_32\n"	
};

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
	uart_send_string("ERROR TYPE ");
    uart_send_string(entry_error_messages[type]);
	uart_send_string("\r\n");
	uart_send_string("Exception class (EC) ");
	uart_send_hex(esr>>26);
	uart_send_string("Instruction specific syndrome (ISS) ");
    uart_send_hex(esr & 0xfff);
}
