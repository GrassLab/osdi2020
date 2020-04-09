#include "include/peripherals/irq.h"
#include "include/irq.h"
#include "include/utils.h"
#include "include/timer.h"
#include "include/uart.h"
#include "include/string.h"

static int core_timer_jiffies = 0;
void irq_handler()
{
	uart_send_string("\r\n");
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
