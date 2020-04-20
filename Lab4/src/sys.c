#include "include/timer.h"
#include "include/uart.h"
#include "include/irq.h"
#include "include/fork.h"
#include "include/mm.h"
#include "include/scheduler.h"

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

unsigned long sys_malloc(){
	unsigned long addr = get_free_page();
	if(!addr){
		return -1;
	}
	return addr;
}

int sys_clone(unsigned long stack){
	return fork(stack);
}

void sys_exit(){
	exit_process();
}

void * const sys_call_table[] = {core_timer,daif,sys_malloc,sys_clone,sys_exit};
