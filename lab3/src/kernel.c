#include "timer.h"
#include "sys.h"
#include "mini_uart.h"
#include "irq.h"

#define CORE0_TIMER_IRQCNTL 0x40000040

int strcmp(char *str1, char *str2) {
    while (1) {
        if (*str1 != *str2) {
            return *str1 - *str2;
        }

        if (*str1 == '\0') {
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
	int buff_size = 100;
	char buffer[buff_size];
	while (1) {
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
			enable_uart_interrupt();
			enable_irq();
			while(1) {

			}
		}
		else if (strcmp(buffer, "irq") == 0) {
			uart_send_string("irq\n");
			sync_call_time();
			uart_send_string("ttt\n");
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
