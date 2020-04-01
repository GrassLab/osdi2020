#include "uart.h"
#include "img_loader.h"
#include "shell.h"

void main() {
    uart_init();
    uart_puts("\nIt's the third mini bootloader, input: loadimg <address offset> to load kernel img.\n\r");
    uart_puts("Default offset: 0, limit: 128 MB. format: hex.\n\r");
    while(1) {
        wait_command();
    }
}