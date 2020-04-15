#include "timer.h"
#include "sys.h"
#include "mini_uart.h"
#include "irq.h"

#define CORE0_TIMER_IRQCNTL 0x40000040
#define BUFFER_SIZE 32
#define CMD_BUFFER_SIZE 32

extern char uart_buffer[BUFFER_SIZE];
extern char cmd_buffer[CMD_BUFFER_SIZE];
extern unsigned int wr_buffer_index;
extern unsigned int rd_buffer_index;
extern unsigned int cmd_index;
extern int cmd_flag ;

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
		
	while (1) {
		if (uart_recv()) {
            break;
		}
	}
	sync_call_uart();
	uart_send_string("uart_init\r\n");
	uart_send_string("# ");
	while (1) {
		if (cmd_flag == 1 && strcmp(cmd_buffer, "hello") == 0) {
            uart_send_string("Hello World!!!\r\n");
			cmd_buffer[0] = '\0';
			cmd_flag = 0;
			uart_send_string("# ");
		}
		else if (cmd_flag == 1 && strcmp(cmd_buffer, "exc") == 0) {
			sync_call_exc();
			cmd_buffer[0] = '\0';
			cmd_flag = 0;
			uart_send_string("# ");
		}
		else if (cmd_flag == 1 && strcmp(cmd_buffer, "irq") == 0) {
			sync_call_time();
			cmd_buffer[0] = '\0';
			cmd_flag = 0;
		}
	}
}
