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

#include "tools.h"
#include "uart.h"
#include "mbox.h"
#include "get_information.h"
#include "gpio.h"
#include "load_kernel.h"

#define PM_PASSWORD (0x5a000000)
#define PM_RSTC ((int*)0x3F10001c)
#define PM_WDOG ((int*)0x3F100024)

void main()
{
	// Initialization
    char buffer[1000];
	char current_input;
    int i1, i2;
    float f1, f2;
    int sizeof_current_line = 0;
    _Bool command_not_found = 1;
	unsigned long long int tmp_CNTFRQ_EL0;
	unsigned long long int tmp_CNTPCT_EL0;
	
	// UART0 initialization
    uart_init();
	uart_puts("\n");
	// Print the peripheral information
	get_serial_number();
	get_board_revision();
	get_VC_memory();

    // Set up serial console
    uart_getc();
    // Welcome message
	uart_puts("\r\nWelcome to raspberry pi 3!\n# "); 

	while(1){
		sizeof_current_line = uart_get_string_with_echo(buffer);
		command_not_found = 1;
		if(!strcmp(buffer, "hello")) {
			// Print Hello World!
			uart_puts("\rHello World!\n");
			command_not_found = 0;
		}
		else if(!strcmp(buffer, "help")) {
			// Print all commands
			uart_puts("\rhello: print Hello World!\n");
			uart_puts("help: help\n");
			uart_puts("reboot: reboot rpi3\n");
			uart_puts("timestamp: get current timestamp\n");
			command_not_found = 0;
		}
		else if(!strcmp(buffer, "timestamp")) {
			// Print timestamp
			asm volatile("mrs %0, CNTPCT_EL0" : "=r" (tmp_CNTPCT_EL0));
			asm volatile("mrs %0, CNTFRQ_EL0" : "=r" (tmp_CNTFRQ_EL0));
			// divide operation
			f1 = (float) ( (float)tmp_CNTPCT_EL0 / (float)tmp_CNTFRQ_EL0 );
			i1 = f1;
			f2 = f1 - i1;	
			while(1) {
				i2 = f2;
				if((float)i2 == f2) break;
				f2 = f2 * 10;
			}
			// turn the number into string
			char buf[100], buf1[100], buf2[100], buf3[100];
			itoa(i1, buf, 10); itoa(i2, buf1, 10);
			itoa(tmp_CNTPCT_EL0, buf2, 10);
			itoa(tmp_CNTFRQ_EL0, buf3, 10);
			uart_puts("\r["); uart_puts(buf); uart_puts(".");
			uart_puts(buf1); uart_puts("]"); uart_puts("\n");
			command_not_found = 0;
		}
		else if(!strcmp(buffer, "reboot")) {
			// Reboot rpi3

			*PM_RSTC = ((PM_PASSWORD) | 0x20); // full reset
  			*PM_WDOG = ((PM_PASSWORD) | 10); // number of watchdog tick
				
			sizeof_current_line = 0;
			command_not_found = 0;
			continue;
		}
		else if(!strcmp(buffer, "framebuffer")) {
			uart_puts("\rShow framebuffer\n");
			command_not_found = 0;
		}
		else if(!strcmp(buffer, "loadimg")) {
			uart_puts("\rLoad kernel image\n");
			char kernel_address_buffer[1000];
			load_kernel_at_target_address(kernel_address_buffer);
			command_not_found = 0;
		}
		if(command_not_found == 1 && sizeof_current_line>0) {
			uart_puts("\rError: command \"");
			uart_puts(buffer);
			uart_puts("\" not found. Try <help>\n");
		}
		uart_puts("\r# ");
		sizeof_current_line = 0;
    }
}
