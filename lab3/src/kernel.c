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

unsigned int read_cntfrq(void)
{
    unsigned int val;
	asm volatile ("mrs %0, cntfrq_el0" : "=r" (val));
    return val;
}

void write_cntp_tval(unsigned int val)
{
	asm volatile ("msr cntp_tval_el0, %0" :: "r" (val));
    return;
}

unsigned int read_cntp_tval(void)
{
    unsigned int val;
	asm volatile ("mrs %0, cntp_tval_el0" : "=r" (val));
    return val;
}

void kernel_main(void)
{	
	int uart_flag = 0;
	int buff_size = 100;
	char buffer[buff_size];
	while (1) {
		if (uart_flag != 1)
			readline(buffer, buff_size);
		if (strcmp(buffer, "hello") == 0) {
            uart_send_string("Hello World!!!\n");
		}
		else if (strcmp(buffer, "exc") == 0) {
			uart_send_string("exc\n");
			sync_call_exc();
		}
		else if (strcmp(buffer, "level") == 0) {
			int el = get_el();
			uart_send_int(el);
		}
		else if (strcmp(buffer, "uart_irq") == 0) {
			uart_send_string("uart_irq\n");
			sync_call_uart();
			int i = 0;
			while(1) {
				if (strcmp(cmd_buffer, "hello") == 0 && cmd_flag == 1) {
					uart_send_string("Hello World!!!\r\n");
					cmd_buffer[0] = '\0';
					cmd_flag = 0;
				}
				else if (cmd_flag == 1) {
					cmd_flag = 0;
				}
			}
		}
		else if (strcmp(buffer, "irq") == 0) {
			uart_send_string("irq\n");
			sync_call_time();
			// // enable system timer
			// sys_timer_init();
			// enable_interrupt_controller();

			// // enable core timer
			// unsigned int cntfrq;
			// unsigned int val;
			// cntfrq = read_cntfrq();
			// write_cntp_tval(cntfrq);    // clear cnthp interrupt and set next 1 sec timer.
			// val = read_cntp_tval();     // read 
			// core_timer_enable();
			// enable_irq();
		}
		else {
			uart_send_string("Wrong Command\n");
		}
	}
}
