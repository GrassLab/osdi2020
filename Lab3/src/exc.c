#include "include/uart.h"

void exception_handler(unsigned long type,unsigned long esr, \
		unsigned long elr,unsigned long spsr, unsigned long far){
       
	switch(type){
		case 0:uart_send_string("\r\nSynchronous");break;		          case 1:uart_send_string("\r\nIRQ");break;
		case 2:uart_send_string("\r\nFIQ");break;	
		case 3:uart_send_string("\r\nSError");break;	
	}
	uart_send_string(":");
	
	// elr: return address
        uart_send_string("\r\nException return address: 0x");
	uart_hex(elr);

	// EC[31:26]: Exception class
	uart_send_string("\r\nException class(EC): 0x");
	uart_hex(esr>>26);

        // ISS[24:0]: Instruction Specific Syndrome
        uart_send_string("\r\nInstruction specific syndrome (ISS): 0x");
	uart_hex(( ((unsigned int)esr)<<7)>>7);

	uart_send_string("\r\n");

}
