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

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int*)(0x3F10001c))
#define PM_WDOG ((volatile unsigned int*)(0x3F100024))
#define PM_RSTS ((volatile unsigned int*)(0x3F100020))

int strcmp(char *s1, char *s2){
    while(*s1 || *s2){
        if(*s1 != *s2)
            return 0;
        ++s1;
        ++s2;
    }
    return 1;
}

double get_time(){
    register double frequency, counter;
    double time;
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(frequency));
    asm volatile ("mrs %0, cntpct_el0" : "=r"(counter));
    time = counter / frequency;
    return time;
}

void send_time(double time){
    unsigned long t1, t2;
    unsigned long max = 1000000000;
    t1 = time;
    t2 = (time-t1)*1000000000;
    uart_send_int(time);
    uart_send('.');
    while(t2 < max/10){
        uart_send_int(0);
        max/=10;
    }
    uart_send_int(t2);
    uart_puts("\r\n");
}

void reset(int tick){ // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20; // full reset
  *PM_WDOG = PM_PASSWORD | tick; // number of watchdog tick
}

void cancel_reset() {
  *PM_RSTC = PM_PASSWORD | 0; // full reset
  *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}

void main()
{
    uart_init();
    uart_puts("Hello RPI3!\n");
    char command[10];
    while(1){
      uart_gets(command);
      if (strcmp(command, "help"))
          uart_puts("help: help\n"
                    "hello: print Hello World!\n"
                    "timestamp: get current timestamp\n"
                    "reboot: reboot rpi3\n");
      else if (strcmp(command, "hello"))
          uart_puts("Hello World!\n");
      else if (strcmp(command, "timestamp")){
          double time;
          time = get_time();
          send_time(time);
      }
      else if (strcmp(command, "reboot")){
          uart_puts("reboot...\r\n");
          reset(100);
      }
      else if (*command)
          uart_puts("error: command not found,  try <help>\n");
    }
}
