#include "mini_uart.h"
#include "simple_shell.h"
char* info ="\n\
   ____    _____    ____     ____\n\
  / __ \\  / ___/   / __ \\   /  _/\n\
 / / / /  \\__ \\   / / / /   / /\n\
/ /_/ /  ___/ /  / /_/ /  _/ /\n\
\\____/  /____/  /_____/  /___/_LJS\n\n";

int main()
{
    uart_init();
    uart_puts(info);
    run_shell();
    return -1;
}
