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

void write_cnthp_tval(unsigned int val)
{
	asm volatile ("msr cnthp_tval_el2, %0" :: "r" (val));
    return;
}

unsigned int read_cnthp_tval(void)
{
    unsigned int val;
	asm volatile ("mrs %0, cnthp_tval_el2" : "=r" (val));
    return val;
}

void kernel_main(void)
{
	uart_init();
	el2_vector_init();
	int buff_size = 100;
	char buffer[buff_size];
	while (1) {
		readline(buffer, buff_size);
		if (strcmp(buffer, "hello") == 0) {
            uart_send_string("Hello World!!!\n");
        }
		else if (strcmp(buffer, "exc") == 0) {
			uart_send_string("exc\n");
			sync_call();
		}
		else if (strcmp(buffer, "level") == 0) {
			int el = get_el();
			uart_send_int(el);
		}
		else if (strcmp(buffer, "irq1") == 0) {
			uart_send_string("irq1\n");
			timer_init();
			enable_interrupt_controller();
			enable_irq();
		}
		else if (strcmp(buffer, "irq") == 0) {
			unsigned int cntfrq;
			unsigned int val;
			uart_send_string("irq\n");
			cntfrq = read_cntfrq();
			uart_send_int(cntfrq);

			write_cnthp_tval(cntfrq);    // clear cnthp interrupt and set next 1 sec timer.
			val = read_cnthp_tval();     // read 
			uart_send_int(val);
			core_timer_enable();
			enable_irq();
		}
		else {
			uart_send_string("Wrong Command\n");
		}
	}
}
