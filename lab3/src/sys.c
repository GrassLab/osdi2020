#include "utils.h"
#include "mini_uart.h"
#include "entry.h"
#include "sys.h"

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
	"ERROR_INVALID_EL0_32"	
};

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
    
    uart_send_hex(address);
    uart_send_hex(esr>>26);
    uart_send_hex(esr & 0xfff);
}
