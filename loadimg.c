#include "include/uart.h"

void loadimg(unsigned int kernel_addr)
{
    unsigned int size=0;

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
        uart_puts("SE");
        return;
    }
    uart_puts("OK");

    uart_puts("Please input the kernel load address (default: 0x80000):\n");
    uart_puts("Please send the kernel from UART...\n");


    uart_puts("Loading kernel at 0x");
    uart_hex(kernel_addr);
    uart_puts(" with size 0x");
    uart_hex(size);
    uart_puts(" ...\n");
    
    // read the kernel
    while(size--) *(char *)kernel_addr++ = uart_getc();
}