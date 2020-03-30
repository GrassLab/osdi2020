#include "uart.h"
#include "string.h"
#include "utility.h"
#include "mailbox.h"
#include "command.h"

int main()
{
    uart_init();
    printPowerOnMessage();
    processCommand();
    
    // uart_puts("This is testing kernel!");
    // uart_puts("# ");
    // char command[1024] = {0};
    // int commandIndex = 0;
    // while (1) {
    //     char c = uart_getc();
    //     if (c == '\n') {
    //         uart_puts("\n");
    //         if (strEqual(command, "loadimg")) {
    //             command_load_image();
    //         }
    //         uart_puts("# ");
    //         memset(command, 0, sizeof(command));
    //         commandIndex = 0;
    //     } else {
    //         uart_send(c);
    //         command[commandIndex] = c;
    //         commandIndex ++;
    //     }
    // }
}