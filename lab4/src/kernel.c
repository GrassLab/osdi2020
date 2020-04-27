#include "timer.h"
#include "sys.h"
#include "mini_uart.h"
#include "irq.h"
#include "fork.h"
#include "../include/sched.h"

void process(char *array) {
	int i = 0;
	while (1) {
		i = 0;
		while(1) {
			if (array[i] == '\0')
				break;
			uart_send(array[i]);
			i++;
		}
		delay(100000000);
		if(current->counter == 0) {
			schedule();
		}
	}
	return;
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
void kernel_main(void)
{	
	char buffer[100];
	uart_recv();
	uart_send_string("uart_init\r\n");
	uart_send_string("# ");
	sync_call_time();
	enable_irq();

	int res = copy_process((unsigned long)&process, (unsigned long)"12345\r\n");
	if (res != 0) {
		uart_send_string("fail to fork\r\n");
	}
	res = copy_process((unsigned long)&process, (unsigned long)"66666\r\n");
	if (res != 0) {
		uart_send_string("fail to fork\r\n");
	}
	while (1) {
		schedule();
	}
}
