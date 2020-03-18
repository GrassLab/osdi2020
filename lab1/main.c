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


void reverse(char str[], int length) { 
    int start = 0; 
    int end = length -1; 
    while (start < end) { 
		char tmp  = *(str+start);
		*(str+start) = *(str+end);
		*(str+end) = tmp;
        //swap(*(str+start), *(str+end)); 
        start++; 
        end--; 
    } 
}
char* itoa(unsigned long long int num, char* str, unsigned long long int base) { 
    unsigned long long int i = 0; 
    _Bool isNegative = 0; 

    if (num == 0) { 
        str[i] = '0';
		i = i + 1;
        str[i] = '\0'; 
        return str; 
    } 
    if (num < 0 && base == 10) { 
        isNegative = 1; 
        num = -num; 
    } 
    while (num != 0) { 
        unsigned long long int rem = num % base; 
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        num = num/base; 
    }  
    if (isNegative) 
        str[i++] = '-'; 
  
    str[i] = '\0';
    reverse(str, i); 
    return str; 
} 
void main()
{
    char buffer[100];
	char current_input;
    int i1, i2, i3;
    float f1, f2, f3;
    int sizeof_current_line = 0;
    _Bool command_not_found = 1;
	unsigned long long int tmp_CNTFRQ_EL0;
	unsigned long long int tmp_CNTPCT_EL0;
	unsigned long long int output_time;
    // set up serial console
    uart_init();
    uart_getc();
    // say hello
    //uart_puts("Hello World!\n");
    uart_puts("\r\nWelcome to raspberry pi 3!\n#"); 

    // echo everything back
    //uart_puts("\r#");
    while(1) {
		current_input = uart_getc();
        buffer[sizeof_current_line] = current_input;
        uart_send(current_input);
		sizeof_current_line = sizeof_current_line + 1;
        if(current_input == '\n') {
			//uart_puts("YOU PRESS ENTER\n");
			command_not_found = 1;
			if(sizeof_current_line==6 && buffer[0]=='h' && buffer[1]=='e' && buffer[2]=='l' && buffer[3]=='l' && buffer[4]=='o') {
				// print Hello World!
				uart_puts("\rHello World!\n");
				command_not_found = 0;
			}
			else if(sizeof_current_line==5 && buffer[0]=='h' && buffer[1]=='e' && buffer[2]=='l' && buffer[3]=='p') {
				// print all commands
				uart_puts("\rhello: print Hello World!\n");
				uart_puts("help: help\n");
				uart_puts("reboot: reboot rpi3\n");
				uart_puts("timestamp: get current timestamp\n");
				command_not_found = 0;
			}
			else if(sizeof_current_line==10 && buffer[0]=='t' && buffer[1]=='i' && buffer[2]=='m' && buffer[3]=='e' && buffer[4]=='s' && buffer[5]=='t' && buffer[6]=='a' && buffer[7]=='m' && buffer[8]=='p' ) {
				// print timestamp
				//uart_puts("timestamp TO BE CONTINUE...\n");
				asm volatile("mrs %0, CNTPCT_EL0" : "=r" (tmp_CNTPCT_EL0));
				asm volatile("mrs %0, CNTFRQ_EL0" : "=r" (tmp_CNTFRQ_EL0));
				//output_time = tmp_CNTPCT_EL0 / tmp_CNTFRQ_EL0;
				f1 = (float) ( (float)tmp_CNTPCT_EL0 / (float)tmp_CNTFRQ_EL0 );
				i1 = f1;
				f2 = f1 - i1;
				
				while(1) {
				    i2 = f2;
				    if((float)i2 == f2) { 
					break;
				    }
				    f2 = f2 * 10;
				}

				char buf[100];
				char buf1[100];
				char buf2[100];
				char buf3[100];
				itoa(i1, buf, 10);
				itoa(i2, buf1, 10);
				itoa(tmp_CNTPCT_EL0, buf2, 10);
				itoa(tmp_CNTFRQ_EL0, buf3, 10);
				uart_puts("\r[");
				uart_puts(buf);
				uart_puts(".");
				uart_puts(buf1);
				uart_puts("]");
				uart_puts("\n");
				//uart_puts("CNTPCT_EL0: ");
				//uart_puts(buf2);
				//uart_puts("\n");
				//uart_puts("CNTFRQ_EL0: ");
				//uart_puts(buf3);
				//uart_puts("\n");

				command_not_found = 0;
			}
			else if(sizeof_current_line==7 && buffer[0]=='r' && buffer[1]=='e' && buffer[2]=='b' && buffer[3]=='o' && buffer[4]=='o' && buffer[5]=='t' ) {
				// reboot rpi3
				//uart_puts("reboot TO BE CONTINUE...\n");
				//uart_puts("\n\r");	

				*PM_RSTC = ((PM_PASSWORD) | 0x20); // full reset
  				*PM_WDOG = ((PM_PASSWORD) | 10); // number of watchdog tick
				
				sizeof_current_line = 0;
				command_not_found = 0;
				continue;
			}
			if(command_not_found == 1) {
				if(sizeof_current_line != 1) { 
					uart_puts("\rError: command \"");
					buffer[sizeof_current_line-1] = '\0';
					uart_puts(buffer);
					uart_puts("\" not found. Try <help>\n");
				}
			}
			uart_puts("\r#");
			sizeof_current_line = 0;
        }
		
    }
}
