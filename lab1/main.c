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
#include "power.h"
#include "delays.h"


void main()
{
    char c;
    unsigned long freq;
    unsigned long counter;
    const char *help = "help";
    const char *hello = "hello";
    const char *time = "time";
    const char *reboot = "reboot";

    // set up serial console
    uart_init();

    while(1) {
	
	char tmp;
	char user_input[10];
	int i = 0;
	uart_send('>');
	while (i < 10) {
            tmp = uart_getc();
            if (tmp == '\n') break;
	    uart_send(tmp);
            user_input[i++] = tmp;
        }
	uart_send('\n');
	if (i == 0) {
	    uart_puts("wrong input\n");
	}

	if (uart_strcmp(user_input, hello) == 0) {
	    uart_puts("hello world\n");
	}
	if (uart_strcmp(user_input, reboot) == 0) {
	    uart_puts("alloha\n");
	}
	
	if (uart_strcmp(user_input, time) == 0) {
            freq = get_timer_freq();
            counter = get_timer_counter();
            
	    uart_puts("[");
	    uart_send(uart_i2c(counter/freq));
	    uart_puts(".");
            int remain = counter % freq;
            int precision = 10;
            while (precision > 0) {
                remain *= 10;
                uart_send(uart_i2c(remain/freq));
                remain %= freq;
                if (remain == 0) break;
                precision -= 1;
            }
            uart_puts("]\n");
        }
	
	if (uart_strcmp(user_input, help) == 0) {
	    uart_puts("help me\n");
        }

    }
}
