#include "include/scheduler.h"
#include "include/signal.h"
#include "include/printf.h"
#include "include/irq.h"

void signal_handler(unsigned long x){
	enable_irq();

	if(current->pending_signal == SIGKILL){
		printf("@@@ Kill task %d \r\n",current->pid);
		exit_process();
	}

	disable_irq();

}

