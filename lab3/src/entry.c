#include "../include/entry.h"
#include "../include/mini_uart.h"

const char *entry_error_messages[] = {
	"SYNC_INVALID_EL2t\n",
	"IRQ_INVALID_EL2t\n",		
	"FIQ_INVALID_EL2t\n",		
	"ERROR_INVALID_EL2T\n",		

	"SYNC_INVALID_EL2h\n",		
	"IRQ_INVALID_EL2h\n",		
	"FIQ_INVALID_EL2h\n",		
	"ERROR_INVALID_EL2h\n",		

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
    uart_send_string(entry_error_messages[type]);
}
