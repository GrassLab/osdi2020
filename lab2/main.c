#include "uart.h"
#include "myfunc.h"
#include "hwinfo.h"
#include "lfb.h"
#include "kernel.h"

void main()
{
    uart_init(4000000);
    uart_puts("loadimg\n");

    char load_addr[11];
    uart_puts("Please input kernel load address (default: 0x80000)\n");
    uart_gets(load_addr);
    uart_puts("Please send kernel image from UART now...\n");
    
    unsigned int kernel_size;
    kernel_size = get_kernel_size();
    uart_puts("Kernel Image Size: ");
    uart_send_int(kernel_size);
    uart_puts("    load Addr: ");
    (*load_addr) ? uart_puts(load_addr) : uart_puts("0x80000");
    uart_puts("...\n");

    unsigned long load_addr_ul;
    load_addr_ul = (*load_addr) ? atoi(load_addr) : 0x80000; 
    // uart_hex(addr_ul);
    // uart_puts("\n");

    unsigned long loader_copy_addr;
    loader_copy_addr = calc_loader_copy_addr(load_addr_ul);
    copy_loader_and_jump((char*)loader_copy_addr, (char*)load_addr_ul, kernel_size);

    recieve_kernel((char*)load_addr_ul, kernel_size);

    uart_puts("[");
    uart_send_double(get_time());
    uart_puts("] recieve done!\n");

    lfb_init();
    lfb_showpicture();

    get_board_revision();
    get_VC_base_addr();
    get_clock_rate(2);

    uart_puts("\n\n\n");
    
    asm volatile ("mov x30, %0; ret" ::"r"(load_addr_ul));
}
