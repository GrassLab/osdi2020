#include "mini_uart.h"
#include "simple_shell.h"


int main()
{
    // set up serial console
    uart_init();
    run_shell();
}
