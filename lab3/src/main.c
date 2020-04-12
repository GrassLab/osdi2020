#include "uart.h"
#include "mbox.h"
#include "shell.h"
#include "framebuffer.h"
#include "timer.h"
#include "irq.h"
#include "config.h"

void init_uart(){
    uart_init();
    get_board_revision();
    get_vc_memory();
    get_UART_clock();
    set_UART_clock();
    get_UART_clock();
}

void init_lfb(){
    lfb_init();
    lfb_showpicture();
}

void main()
{
    init_uart();
    init_lfb();
    init_printf(0, putc);

    irq_vector_init();
    // timer_init();
    enable_interrupt_controller();
	enable_irq();
    shell();

    // echo everything back
    while(1) {
        uart_send(uart_getc());
    }

}
