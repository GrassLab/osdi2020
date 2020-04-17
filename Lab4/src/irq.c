#include "include/irq.h"
#include "include/utils.h"
#include "include/timer.h"
#include "include/uart.h"
#include "include/string.h"
#include "include/sys.h"

#define IRQ_PENDING_1 0x3F00B204
#define IRQ_PENDING_2 0x3F00B208
#define CORE_SOURCE_0 0x40000060

unsigned int core_timer_jiffies = 0;
unsigned int sys_timer_jiffies = 0;

void irq_handler()
{	
	unsigned int irq = get32(IRQ_PENDING_1);
	unsigned int irq_zero = get32(CORE_SOURCE_0);
	unsigned int irq_two = get32(IRQ_PENDING_2);
//      // print for debug	
	//uart_send_string("\r\n");
	//uart_hex(irq);
	//uart_hex(irq_zero);
	//uart_hex(irq_two);

	if (irq==2){
		//for system timer interrupt
		sys_timer_handler();

		if (sys_timer_jiffies>0){
			uart_send_string("System timer interrupt, jiffies, ");
			
			char buffer[16];
			itos(sys_timer_jiffies,buffer,10);
			uart_send_string(buffer);
			uart_send_string("\r\n");
		}
		sys_timer_jiffies++;
	}
		
	else if(irq_zero==2){
		core_timer_handler();

		if (core_timer_jiffies>0){
			uart_send_string("Core timer interrupt, jiffies, ");
			char buffer[16];
			itos(core_timer_jiffies,buffer,10);
			uart_send_string(buffer);
			uart_send_string("\r\n");
		}
		core_timer_jiffies++;
	}		
	else if(irq_two == 0x02000000){
		uart_IRQhandler();	
	}
	else{
		uart_send_string("Unknown IRQ\r\n");
	}
	
	//delay(10000000);	
}
