#include "uart.h"
#include "string.h"
#include "utility.h"

void processCommand(char *command) {
    if (strEqual(command, "help")) {
        uart_puts("hello: print hello word\n");
        uart_puts("help: help\n");
        uart_puts("reboot: reboot pi\n");
        uart_puts("timestamp: get current timestamp\n");
    } else if (strEqual(command, "hello")) {
        uart_puts("Hello World!\n");
    } else if (strEqual(command, "reboot")) {
        reboot(0);
        return;
    } else if (strEqual(command, "timestamp")) {
        char str[1024] = {0};
        doubleToStr(getTimestamp(), str);
        uart_puts("[");
        uart_puts(str);
        uart_puts("]");
        uart_puts("\n");
    } else if (strlen(command) != 0){
        uart_puts("Error: command ");
        uart_puts(command);
        uart_puts(" not found\n");
    }
    uart_puts("# ");
}

int main()
{
    uart_init();
    printPowerOnMessage();
    uart_puts("# ");

    char command[1024] = {0};
    int commandIndex = 0;
    int isbooting = 1;
    while (1) {
        char c = uart_getc();
        if (isbooting) {
            isbooting = 0;
            continue;
        }
        if (c == '\n') {
            uart_puts("\n");
            processCommand(command);
            memset(command, 0, sizeof(command));
            commandIndex = 0;
        } else {
            uart_send(c);
            command[commandIndex] = c;
            commandIndex ++;
        }
    }
}