/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "uart.h"
#define PM_PASSWORD (0x5a000000)
#define PM_RSTC ((int*)0x3F10001c)
#define PM_WDOG ((int*)0x3F100024)

int str_cmp (char *str1, char *str2) {
	for (int i = 0; ; i++) {
        if (str1[i] != str2[i]) {
            return str1[i] < str2[i] ? -1 : 1;
        }

        if (str1[i] == '\0'){
            return 0;
        }
    }
}

void main () {
	char buf[50];
	char cur_input;
	int cur_len = 0;
	unsigned long long int tmp_CNTFRQ_EL0;
	unsigned long long int tmp_CNTPCT_EL0;
	float f1;
	int i1;


	uart_init();

	uart_puts("\r\nWelcome\n# ");

	while (1) {
		cur_input = uart_getc();
		uart_send(cur_input);
		buf[cur_len++] = cur_input;
		buf[cur_len] = '\0';

		if (cur_input == '\n') {
			buf[--cur_len] = '\0'; // convert '\n' into '\0'

			if (str_cmp(buf, "hello") == 0) {
				uart_puts("\rHello World!\n");
			}
			else if (str_cmp(buf, "help") == 0) {
				uart_puts("\rhello: print Hello World!\n");
				uart_puts("help: help\n");
				uart_puts("reboot: reboot rpi3\n");
				uart_puts("timestamp: get current timestamp\n");
			}
			else if (str_cmp(buf, "timestamp") == 0) {
				asm volatile("mrs %0, CNTPCT_EL0" : "=r" (tmp_CNTPCT_EL0));
				asm volatile("mrs %0, CNTFRQ_EL0" : "=r" (tmp_CNTFRQ_EL0));
				f1 = (float)tmp_CNTPCT_EL0 / (float)tmp_CNTFRQ_EL0;

				i1 = (int)f1;
				f1 = f1 - i1;

				while(f1 - (int)f1) {
					f1 *= 10;
				}

				//TODO

			}
			else if (str_cmp(buf, "reboot") == 0) {
				uart_puts("reboot...\n");
				uart_puts("\n\r");	

				*PM_RSTC = ((PM_PASSWORD) | 0x20); // full reset
  				*PM_WDOG = ((PM_PASSWORD) | 5); // number of watchdog tick
			}
			else { // not one of the commands above
				if (cur_len > 1) { // and not pure '\n'
					uart_puts("\rError: command \"");
					for (int i = 0; i < cur_len; i++) {
						uart_send(buf[i]);
					}
					uart_puts("\" not found. Try <help>\n");
				}
			}

			uart_puts("\r# ");

			// reset buffer and cur_len
			cur_len = 0;
		}

    } 

}