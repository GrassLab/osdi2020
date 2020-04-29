#include "timer.h"
#include "sys.h"
#include "mini_uart.h"
#include "irq.h"
#include "fork.h"
#include "../include/sched.h"

void user_process1(char *array) {
	// this process run at el0
	while(1) {
		uart_send_string(array);
		delay(100000000);
	}
}

void user_process() {
	//system call for clone process
	sync_call_clone((unsigned long)user_process1, "Hello world!\r\n");
	while(1) {
		uart_send_string("ppppp\r\n");
		delay(100000000);
	}
}


int strcmp(char *str1, char *str2) {
    while (1) {
        if (*str1 != *str2) {
            return *str1 - *str2;
        }

        if (*str1 == '\0' && *str2 != '\0') {
            return 1;
        }
		else if (*str1 != '\0' && *str2 == '\0') {
			return 1;
		}
		else if (*str1 == '\0' && *str2 == '\0') {
			return 0;
		}
        str1++;
        str2++;
    }
}

void kernel_process()
{
	uart_send_string("Kernel process start at EL: ");
	unsigned long err = do_exec((unsigned long)user_process);
}

void kernel_main(void)
{	
	uart_recv();
	uart_send_string("uart_init\r\n");
	uart_send_string("# ");
	sync_call_time();
	
	int res = copy_process(PF_KTHREAD, (unsigned long)kernel_process, 0, 0); //kernel init task fork the process 
	if (res == -1) {
		uart_send_string("fail to fork\r\n");
	}
	while (1) {
		schedule();
	}
}
