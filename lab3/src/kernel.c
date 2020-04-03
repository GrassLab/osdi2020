#include "timer.h"
#include "sys.h"
#include "mini_uart.h"

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

void kernel_main(void)
{
	uart_init();
	sync_vector_init();
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
		else {
			uart_send_string("Wrong Command\n");
		}
	}
}
