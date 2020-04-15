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
#include "mystd.h"
#include "timer.h"
#include "exc.h"
#include "irq.h"

#define CMDSIZE 64
char cmd[CMDSIZE] = {0};
int cmdSize = 0;



void cmd_process(){
    uart_puts("\r");

    if(strcmp(cmd, "exc")){
        sysCall_print_esr_elr();
    }else if(strcmp(cmd, "brk")){
        brk_instr();
    }else if(strcmp(cmd, "timer")){
        sysCall_set_timer();
    }else if(strcmp(cmd, "distimer")){
        sysCall_unset_timer();
    }else if(strcmp(cmd, "uartirq")){
        sysCall_miniUART_irq();
    }else if(strcmp(cmd, "el")){
        show_currentEL();
    }else if(strcmp(cmd, "help")){
        uart_puts("command: \"help\" Description: \"print all available commands\"  \n");
        uart_puts("command: \"exc\" Description: \"\"exception handler should print the return address, EC field, and ISS field.\n");
        uart_puts("command: \"timer\" Description: \"enable timer irq\"  \n");
        uart_puts("command: \"distimer\" Description: \"disable timer irq\"  \n");
        uart_puts("command: \"uartirq\" Description: \"enable mini uart irq\"  \n");
        uart_puts("command: \"el\" Description: \"print current exception level, not allow in el0\"  \n");
    } else if(strlen(cmd) != 0){
        uart_puts("command \"");
        uart_puts(cmd);
        uart_puts("\" not found, try <help> \n");   
    }

    uart_puts("# ");
}

void cmd_push(char c){
    if(cmdSize<CMDSIZE) {
        switch(c){
            case 8:     //backspace
                if(cmdSize>0) cmdSize--;
                uart_send(32);
                uart_send(8);
                break;
            case 10:    // 0X0A '\n' newline,  0X0D '\r' carriage return
                cmd[cmdSize] = '\0';
                cmdSize++;
                cmd_process();
                cmdSize = 0;
                break;

            default :
                cmd[cmdSize] = c;
                cmdSize++;  
        }
    }
}

void main()
{
    // set up serial console


    uart_init();

    uart_puts("\r\n# ");

    while(1){
        char c = uart_getc();

        if(c < 127){
            uart_send(c);       //show character user typed on uart terminal
            // printASCII(c);
            cmd_push(c);
        }

    }
}
