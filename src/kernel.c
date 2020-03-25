/* 
 * kernel.c
 * Entry point, ker_main is executed right after the memory setup
 */

#include "uart.h"
#include "shell.h"

void ker_main() {

    uart_setup();
    
    shell_start();
}
