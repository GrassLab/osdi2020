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
#include "reboot.h"

void setstr(char* s, char c, int size);
int cmpstr(const char* a, const char* b);
void cpystr(char* a, char* b, int size);
char* itoa(unsigned int val, char* str);
void swap(char* a, char* b);
char* revstr(char* str, int start, int end);
void cmd_help();
void cmd_time();
void cmd_reboot();
void cmd_err(char* buf);
void cmd_debug(char* buf);

void main()
{
    // set up serial console
    uart_init();
    
    // say hello
	uart_puts("\n\n\n");
	uart_puts("                                        |\\__/,|   (`\\  \n");
	uart_puts("                                      _.|o o  |_   ) ) \n");
	uart_puts("|************************************(((***(((********|\n");
    uart_puts("|* Hello World! This is OSDI Lab 1 from Waylon Shih! *|\n");
	uart_puts("|*****************************************************|\n");
    uart_puts("\n\n# ");
    // simple shell
	char buf[40];
	char history[40];
	int len = 0;

	setstr(buf, 0, 40);
	setstr(history, 0, 40);

    while(1) {
		char c = uart_getc();
		// newline, enter command
		if(c == '\n') {
			uart_puts("\r\n");
			// read command
			if(!cmpstr(buf, "help"))
				cmd_help();
			else if(!cmpstr(buf, "hello"))
				uart_puts("Hello world!\n");
			else if(!cmpstr(buf, "timestamp"))
				cmd_time();
			else if(!cmpstr(buf, "reboot"))
				cmd_reboot();
			else if(!cmpstr(buf, "")) ;
			else
				cmd_err(buf);
			
			// reset buffer
			cpystr(history, buf, len);
			setstr(buf, 0, len);
			len = 0;

			uart_puts("# ");
		}
		// backspace
		else if(c == 127) {
			if(len > 0) {
				uart_puts("\b \b");
				buf[--len] = 0;
			}
		}
		// arrow keys
		else if(c == 27 && uart_getc() == '[') {
			char dir = uart_getc();
			// up
			if(dir == 'A');
			// down
			else if(dir == 'B');
			// right
			else if(dir == 'C');
			// left
			else if(dir == 'D');
		}
		else {
			uart_send(c);
			buf[len++] = c;
		}
    }
}

void setstr(char* s, char c, int size) {
	for(int i=0; i<size; i++)
		s[i] = c;
}

int cmpstr(const char* a, const char* b) {
	while(*a) {
		if(*a != *b)
			break;
		a++;
		b++;
	}
	// return ASCII difference
	return *(const unsigned char*)a - *(const unsigned char*) b;
}

// copy string a to string b
void cpystr(char* a, char* b, int size) {
	for(int i=0; i < size; i++)
		a[i] = b[i];
}

// convert unsigned integer into string, decimal format
char* itoa(unsigned int val, char* str) {
	int i = 0;
	while(val) {
		str[i++] = val % 10 + '0';
		val /= 10;
	}
	str[i] = '\0';
	return revstr(str, 0, i-1);
}

// convert fractional part into string, decimal format
// fractional & denominator
char* ftoa(unsigned int frac, unsigned int deno, char* str) {
	int i = -1;
	while(frac && i < 9) {
		if(frac/deno == 0) {
			str[i++] = '0';
			frac *= 10;
		}
		else {
			str[i++] = frac/deno + '0';
			frac %= deno;
			frac *= 10;
		}
	}
	str[i] = '\0';
	return str;
}

// for itoa()
void swap(char* a, char* b) {
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

// reverse string
// for itoa()
char* revstr(char* str, int start, int end) {
	while(start < end)
		swap(&str[start++], &str[end--]);
	return str;
}

/* simple shell functions */
void cmd_help() {
	uart_puts("hello     : print Hello world!\n");
	uart_puts("help      : help\n");
	uart_puts("reboot    : reboot rpi3\n");
	uart_puts("timestamp : get current timestamp\n");
}

// DONE!!!!!!!!!!
void cmd_time() {
	unsigned long long count;
	unsigned long long freq;
	// get register value
	asm volatile("mrs %0, cntpct_el0" : "=r" (count));
	asm volatile("mrs %0, cntfrq_el0" : "=r" (freq));

	unsigned int quot = count / freq;
	unsigned int rem = count % freq;
	char left[20];
	char right[20];

	uart_puts("[");
	uart_puts(itoa(quot, left)); // integer part
	uart_puts(".");
	uart_puts(ftoa(rem, freq, right)); // fractional part
	uart_puts("]\n");
	
}

void cmd_reboot() {
	uart_puts("Reboot . . .\n");
	reset(100);
	do{asm volatile("nop");} while(1);

}

void cmd_err(char* buf) {
	uart_puts("Err: command \'");
	uart_puts(buf);
	uart_puts("\' not found, try <help>.\n");
	//cmd_debug(buf);
}

void cmd_debug(char* buf) {
	char tmp[5];
	while(*buf) {
		uart_puts(itoa((int)*buf, tmp));
		uart_puts(" ");
		buf++;
	}
	uart_puts("\n");
}
	
