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
#include "cmd.h"
#include "type.h"

const int MAX_BUFFER_SIZE = 512;

void main()
{
    // set up serial console
    uart_init();
    
    // say hello
    uart_puts("Welcome to OSDI shell!\n");
    uart_puts("> ");
    
    char buf[MAX_BUFFER_SIZE];
    int buf_ptr = 0; 

    // echo everything back
    while(1) {
        char in_c = uart_getc();
        uart_send(in_c);

        if (in_c == '\n') {
            buf[buf_ptr] = '\0';

            processCmd(buf);
            uart_puts("> ");
            
            buf_ptr = 0;
            buf[buf_ptr] = '\0';
        }
        else {
            buf[buf_ptr++] = in_c;
        }
    }
}
