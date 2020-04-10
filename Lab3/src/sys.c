#include "include/timer.h"
#include "include/uart.h"

void syscall_success_message(){
   
     unsigned long el;
     // read the current level from system register
   
     asm volatile ("mrs %0, CurrentEL" : "=r" (el));
     uart_send_string("### Current EL is: ");
     uart_hex((el>>2)&3);
     uart_send_string("\r\n");

     uart_send_string("System call success!!");
}

void print_this(){
	uart_send('\r');
	uart_send('\n');
	uart_send('!');
}

void sys_timer(char* x){
	uart_send_string(x);	
	core_timer_enable();	
}

void * const sys_call_table[] = {sys_timer};
