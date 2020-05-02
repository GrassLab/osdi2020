#include "exce.h"
#include "mailbox.h"
#include "printf.h"
#include "sched.h"
#include "shell.h"
#include "uart.h"


int kernel_main()
{
    uart_init();
    init_printf(0, uart_putc);  
    get_board_info();
    sched_init(); // create init

    // run_shell();
    return -1;
}
