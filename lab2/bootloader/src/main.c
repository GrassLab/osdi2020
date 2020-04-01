#include "uart.h"
#include "bootloader.h"

void main()
{
    // set up serial console
    uart_init();

    // load kernel by USB-to-TTL
    loadimg();
}