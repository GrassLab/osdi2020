#include "uart.h"
#include "myfunc.h"
#include "hwinfo.h"
#include "lfb.h"

void main()
{
    uart_init(4000000);
    uart_puts("loadimg\n");
    // uart_puts("Please input kernel load address (default: 0x80000)\n");
    uart_puts("Please send kernel image from UART now...\n");

    // read the kernel's size
    int size = 2248;
    size = uart_getc();
    size |= uart_getc()<<8;
    size |= uart_getc()<<16;
    size |= uart_getc()<<24;
    uart_puts("Kernel Image Size: ");
    uart_send_int(size);
    uart_puts("    load Addr: 0x80000\n");
     
    char *kernel=(char*)0x80000;
    // read the kernel
    while(size--) 
        *kernel++ = uart_getc();

    uart_puts("[");
    uart_send_double(get_time());
    uart_puts("] recieve done!\n");

    lfb_init();
    lfb_showpicture();

    get_board_revision();
    get_VC_base_addr();
    get_clock_rate(2);

    uart_puts("\n\n\n");
    
    // restore arguments and jump to the new kernel.
    asm volatile (
        "mov x0, x10;"
        "mov x1, x11;"
        "mov x2, x12;"
        "mov x3, x13;"
        // we must force an absolute address to branch to
        "mov x30, 0x80000; ret"
    );
}
