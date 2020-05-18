#include <stddef.h>
#include <stdint.h>

#include "printf.h"
#include "utils.h"
#include "timer.h"
#include "irq.h"
#include "fork.h"
#include "sched.h"
#include "mini_uart.h"
#include "sys.h"

extern unsigned long user_begin;
extern unsigned long user_end;
extern void user_process();
void kernel_process(){

	asm volatile ("adrp x0, idmap_dir");
	asm volatile ("msr ttbr0_el1, x0");
	// printf("Kernel process started. EL %d\r\n", get_el());
	unsigned long begin = (unsigned long)&user_begin;
	unsigned long end = (unsigned long)&user_end;
	unsigned long process = (unsigned long)&user_process;
	int err = privilege_task_create(begin, end - begin, process - begin); // virtual address of begin & size
	if (err < 0){
		printf("Error while moving process to user mode\n\r");
	} 
}


void kernel_main()
{
	uart_init();
	init_printf(NULL, putc);
	irq_vector_init();
	enable_core_timer();

	int res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0);
	enable_irq();
	if (res < 0) {
		printf("error while starting kernel process");
		return;
	}

	while (1){
		free_zombie_task();
		schedule();
	}	
}
