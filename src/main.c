#include "frame_buffer.h"
#include "mailbox.h"
#include "printf.h"
#include "simple_shell.h"
#include "uart.h"
#include "string.h"

int main()
{
    uart_init();
    init_printf(0, uart_putc);  
    get_board_info();
    run_shell();
    return -1;
}
