#include "include/timer.h"
#include "include/uart.h"
#include "include/irq.h"

int core_timer(){
	core_timer_enable();
	return 10;
}

void daif(){
	unsigned int daif;
        asm volatile ("mrs %0, daif" : "=r" (daif)); 
        uart_send_string("DAIF is: ");
        uart_hex(daif);
        uart_send_string("\r\n");
}

void * const sys_call_table[] = {core_timer,daif};
