#include "include/timer.h"
#include "include/uart.h"

void print_this(unsigned int x){
	uart_send_string("trigger");
	uart_hex(x);
	uart_send_string("\r\n");
}

int core_timer(){
	core_timer_enable();
	return 10;
}

void sys_timer(){
	sys_timer_init();
}

void * const sys_call_table[] = {core_timer,sys_timer};
