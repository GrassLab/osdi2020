#include "include/irq.h"
#include "include/utils.h"
#include "include/timer.h"
#include "include/uart.h"
#include "include/string.h"

#define IRQ_PENDING_1 0x3F00B204
#define CORE_SOURCE_0 0x40000060

unsigned int core_timer_jiffies = 0;
unsigned int sys_timer_jiffies = 0;

void irq_handler()
{	
	uart_send_string("Look:");
	uart_hex(core_timer_jiffies);
	uart_send_string("\r\n");

	unsigned int irq = get32(IRQ_PENDING_1);
	unsigned int irq_zero = get32(CORE_SOURCE_0);
//      // print for debug	
//	uart_send_string("\r\n");
//	uart_hex(irq);
//	uart_hex(irq_zero);

	switch(irq){
	
	    case 2://for system timer interrupt
		sys_timer_handler();

		if (sys_timer_jiffies>0){
			uart_send_string("System timer interrupt, jiffies, ");
			
			char buffer[16];
			itos(sys_timer_jiffies,buffer,10);
			uart_send_string(buffer);
			uart_send_string("\r\n");
		}
		sys_timer_jiffies++;
		break;
	    default:
		switch(irq_zero){
			case 2:
				core_timer_handler();

				if (core_timer_jiffies>0){
					uart_send_string("Core timer interrupt, jiffies, ");
					char buffer[16];
					itos(core_timer_jiffies,buffer,10);
					uart_send_string(buffer);
					uart_send_string("\r\n");
				}
				core_timer_jiffies++;
				break;
	    		default:
				uart_send_string("Unknown IRQ\r\n");
	
		}
	}
	
}
