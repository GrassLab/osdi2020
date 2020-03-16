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
//#include "power.h"
#include "delays.h"


#define PM_PASSWORD 0x5a000000
#define PM_RSTC (int *)0x3F10001c
#define PM_WDOG (int *)0x3F100024
#define set(a, b) *a = b

void reset(int tick)
{ // reboot after watchdog timer expire
  set(PM_RSTC, PM_PASSWORD | 0x20); // full reset
  set(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick
}

void cancel_reset() 
{
  set(PM_RSTC, PM_PASSWORD | 0); // full reset
  set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick
}

void get_time() 
{
    unsigned long freq = get_timer_freq();
    unsigned long counter = get_timer_counter();
    char timestamp[10];

    timestamp[0] = '[';
    timestamp[1] = uart_i2c(counter/freq);
    timestamp[2] = '.';
    int remain = counter % freq;
    int precision = 5;
    int i = 2; 
    while ((precision > 0) & (i < precision)) {
        remain *= 10;
        timestamp[i++] = uart_i2c(remain/freq);
        remain %= freq;
        if (remain == 0) break;
        precision -= 1;
    }
    timestamp[i++] = ']';
    timestamp[i] = '\n';
}

void main()
{
    const char *help = "help";
    const char *hello = "hello";
    const char *time = "time";
    const char *reboot = "reboot";

    // set up serial console
    uart_init();

    while(1) {
	
        char tmp;
        char user_input[12];
        int i = 0;
        uart_send('>');
        while (i < 10) {
            tmp = uart_getc();
            if (tmp == '\n') break;
            uart_send(tmp);
            user_input[i++] = tmp;
        }
        user_input[i] = '\0';
        uart_send('\n');
        //uart_send(uart_i2c(i));
        //uart_puts(user_input);
        if (i == 0) {
            uart_puts("wrong input\n");
        }

        if (uart_strcmp(user_input, hello) == 0) {
            uart_puts("hello world\n");
        }
        if (uart_strcmp(user_input, reboot) == 0) {
            get_time();
            reset(3);
            get_time();
        }
        
        if (uart_strcmp(user_input, time) == 0) {
            get_time();    
        }
        if (uart_strcmp(user_input, help) == 0) {
            uart_puts("help me\n");
        }
    }
}
