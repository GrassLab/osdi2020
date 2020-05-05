#include "include/scheduler.h"
#include "include/signal.h"
#include "include/printf.h"
#include "include/irq.h"

void signal_handler(unsigned long x){
	preempt_disable();
	enable_irq();

	if(current->signal.pending == SIGKILL &&  \
			current->signal.block !=SIGKILL){

		//printf("@@@ Kill task %d \r\n",current->pid);
		current->signal.block = SIGKILL; // It avoid signal handler interrupt itself
		exit_process();
	}

	disable_irq();
        preempt_enable();
}

