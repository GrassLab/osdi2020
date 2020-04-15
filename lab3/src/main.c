#include "uart.h"
#include "mystd.h"
#include "hwinfo.h"
#include "lfb.h"
#include "kernel.h"
#include "timer.h"

#define CMDMAX 32

void main()
{
    // mini_uart_init();
    PL011_uart_init(4000000);
    // uart_puts("Hello RPI3!\n");
    show_boot_msg();
    char command[CMDMAX];
    while(1){
        uart_puts("# ");
        uart_gets(command);
        if (strcmp(command, "help"))
          uart_puts("help: help\n"
                    "timestamp: get current timestamp\n"
                    "reboot: reboot rpi3\n"
                    "mini_UART: switch to mini UART\n"
                    "PL011_UART: switch to PL011 UART\n"
                    "uart_clock_rate: get uart clock rate\n"
                    "hw_info: get board revision, VC Core base address\n"
                    "framebuffer: show a splash image\n"
                    "loadimg: load kernel image to specified address\n"
                    "exc: issues svc #1\n"
                    "irq_timer: enable timer interrupt\n"
                    "irq_uart0: enable PL011 UART interrupt\n");
        else if (strcmp(command, "timestamp")){
          uart_puts("[");
          uart_double(get_time());
          uart_puts("]\n");
        }
        else if (strcmp(command, "reboot")){
            uart_puts("reboot...\n");
            reset(100);
        }
        else if (strcmp(command, "mini_UART")){
            mini_uart_init();
            uart_puts("mini uart init done\n");
        }    
        else if (strcmp(command, "PL011_UART")){
            PL011_uart_init(4000000);
            uart_puts("PL011 uart init done\n");
        }
        else if  (strcmp(command, "uart_clock_rate"))
            get_clock_rate(2);
        else if (strcmp(command, "hw_info")){
            get_board_revision();
            get_VC_base_addr();
        }
        else if (strcmp(command, "framebuffer")){
            lfb_init();
            lfb_showpicture();
        }
        else if (strcmp(command, "loadimg"))
            load_kernel_img();
        else if(strcmp(command, "exc"))
            asm volatile ("svc #1");
        else if(strcmp(command, "irq_timer"))
            asm volatile ("svc #2");
        else if(strcmp(command, "irq_uart0"))
            enable_uart0_irq();
        else if (*command)
            uart_puts("error: command not found,  try <help>\n");
    }
}
