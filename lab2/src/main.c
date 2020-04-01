#include "uart.h"
#include "mbox.h"
#include "shell.h"
#include "framebuffer.h"

void main()
{
    // set up serial console
    lfb_init();
    lfb_showpicture();

    uart_init();
    get_board_revision();
    get_vc_memory();
    get_UART_clock();
    set_UART_clock();
    get_UART_clock();

    shell();

    // echo everything back
    while(1) {
        uart_send(uart_getc());
    }

}
