#include "img_loader.h"
#include "uart.h"
#include "shell.h"

void load_kernel(unsigned long adr) {
    uart_puts("\nPlease input your image file...\r");
    write_file((char*)adr);
    uart_puts("\nfinished loading, it will now start at your image.\r");
    uart_puts("\n");
    timestamp("");
    register unsigned long adr_reg asm("x10");
    adr_reg = adr;
    asm volatile(
        "br x10\n"
    );
    return;
}