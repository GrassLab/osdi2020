#include "uart.h"
#include "lfb.h"

void main()
{
    int size=0;
    
    char *kernel=(char*)0x80000;
    //if(offset) while(offset--) { asm volatile("nop"); }

    // set up serial console and linear frame buffer
    uart_init();
    lfb_init();

    // display a pixmap
    lfb_showpicture();

again:
    // magic number
    uart_puts("HANK0438\n");
    uart_puts("Please send the kernel size...\n");
    // read the kernel's size
    size=uart_getc();
    size|=uart_getc()<<8;
    size|=uart_getc()<<16;
    size|=uart_getc()<<24;

    // send negative or positive acknowledge
    if(size<64 || size>1024*1024) {
        // size error
        uart_send('S');
        uart_send('E');
        goto again;
    }
    uart_send('O');
    uart_send('K');

    uart_puts("Please input the kernel load address (default: 0x80000):\n");
    uart_puts("Please send the kernel from UART...\n");

    // read the kernel
    while(size--) *kernel++ = uart_getc();

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
