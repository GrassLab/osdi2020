#include "uart.h"
#include "rpifunc.h"
#include "shell.h"

void main()
{
    // mini_uart_init();
    PL011_uart_init(4000000);
    // uart_puts("Hello RPI3!\n");
    show_boot_msg();
    run_shell();
}
