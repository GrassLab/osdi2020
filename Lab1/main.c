#include "uart.h"
#include "shell.h"

void main() {
    uart_init();
    sys_init();
    uart_puts("\nThe mini shell is ready.\n");
    while(1) {
        wait_command();
    }
}