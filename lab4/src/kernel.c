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
		delay(1000000000);
		schedule();
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
	// enable timer
	uart_send_string("uart_init\r\n");
	uart_send_string("# ");
	while (1) {
		readline(buffer, 100);
		if (strcmp(buffer, "hello") == 0) {
			uart_send_string("hhhhhh\r\n");
			int res = copy_process((unsigned long)&process, (unsigned long)"12345\r\n");
			if (res != 0) {
				uart_send_string("fail to fork\r\n");
			}
			res = copy_process((unsigned long)&process, (unsigned long)"66666\r\n");
			if (res != 0) {
				uart_send_string("fail to fork\r\n");
			}
			while(1) {
				uart_send_string("scheduling\r\n");
				schedule();
			}
			buffer[0] = '\0';
			uart_send_string("# ");
		}
		else if (strcmp(buffer, "exc") == 0) {
			sync_call_exc();
			buffer[0] = '\0';
			uart_send_string("# ");
		}
		else if (strcmp(buffer, "irq") == 0) {
			sync_call_time();
			buffer[0] = '\0';
			uart_send_string("# ");
		}
		else if (strcmp(buffer, "proc") == 0) {
			sync_call_proc();
			buffer[0] = '\0';
			uart_send_string("# ");
		}
		else {
			uart_send_string("wrong command!!!\r\n");
			uart_send_string("# ");
		}
	}
}
