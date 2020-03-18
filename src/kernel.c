/* 
 * kernel.c
 * Entry point, ker_main is executed right after the memory setup
 */

#include "uart.h"
#include "shell.h"

void ker_main() {

    uart_setup();

    uart_puts("               _                            _                                     \r\n");
    uart_puts("              | |                          | |                                    \r\n");
    uart_puts(" __      _____| | ___ ___  _ __ ___   ___  | |__   ___  _ __ ___   ___            \r\n");
    uart_puts(" \\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ | '_ \\ / _ \\| '_ ` _ \\ / _ \\\r\n");
    uart_puts("  \\ V  V /  __/ | (_| (_) | | | | | |  __/ | | | | (_) | | | | | |  __/          \r\n");
    uart_puts("   \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___| |_| |_|\\___/|_| |_| |_|\\___|   \r\n");

    shell_start();

}
