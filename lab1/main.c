#include "uart.h"

void main()
{
    uart_init();
    uart_puts("Hello World!\n");
    uart_puts("  _     _   _ _ _    __        __         _     _ \n");
    uart_puts(" | |__ | | | | | | __\\ \\      / /__  _ __| | __| |\n");
    uart_puts(" | '_ \\| |_| | | |/ _ \\ \\ /\\ / / _ \\| '__| |/ _` |\n");
    uart_puts(" | | | |  _  | | | (_) \\ V  V / (_) | |  | | (_| |\n");
    uart_puts(" |_| |_|_| |_|_|_|\\___/ \\_/\\_/ \\___/|_|  |_|\\__,_|\n");

    while(1) {
        uart_sendc(uart_getc());
    }
}
