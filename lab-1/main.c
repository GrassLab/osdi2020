#include "uart.h"
#include "string.h"
#include "utility.h"

void processCommand(char *command) {
    if (strEqual(command, "help")) {
        uart_print("hello: print hello word\r\n");
        uart_print("help: help\r\n");
        uart_print("reboot: reboot pi\r\n");
        uart_print("timestamp: get current timestamp\r\n");
    } else if (strEqual(command, "hello")) {
        uart_print("Hello World!\r\n");
    } else if (strEqual(command, "reboot")) {
        uart_print("reboot!\r\n");
    } else if (strEqual(command, "timestamp")) {
        long long int cntfrq;
        asm volatile ("mrs %0, cntfrq_el0" : "=r" (cntfrq));
        long long int cntptc;
        asm volatile ("mrs %0, cntpct_el0" : "=r" (cntptc));
        char str[1024];
        memset(str, 0, sizeof(str));
        floatToStr((double)cntptc / cntfrq, str);
        uart_print(str);
        uart_print("\r\n");
    } else if (!strEqual(command, "")){
        uart_print("Error: command ");
        uart_print(command);
        uart_print(" not found\r\n");
    }
}


int main()
{
    uart_init();
    uart_puts("# ");
    char command[1024];
    int commandIndex = 0;
    memset(command, 0, sizeof(command));
    while(1) {
        char c = uart_getc();
        if (c == '\n') {
            uart_print("\r\n");
            processCommand(command);
            uart_print("# ");
            memset(command, 0, sizeof(command));
            commandIndex = 0;
        } else {
            uart_send(c);
            command[commandIndex] = c;
            commandIndex ++;
        }
    }
}