#include "uart.h"

void main()
{
    uart_init();
    printf("Enter mini-shell: \n");
    printf("  _   _        _  _          ____                     _  \n");
    printf(" | | | |  ___ | || |  ___   |  _ \\  __ _  ___  _ __  (_)\n");
    printf(" | |_| | / _ \\| || | / _ \\  | |_) |/ _` |/ __|| '_ \\ | |\n");
    printf(" |  _  ||  __/| || || (_) | |  _ <| (_| |\\__ \\| |_) || |\n");
    printf(" |_| |_| \\___||_||_| \\___/  |_| \\_\\\\__,_||___/| .__/ |_|\n");
    printf("                                              |_|       \n");


    while(1) {
        uart_sendc(uart_getc());
    }
}
