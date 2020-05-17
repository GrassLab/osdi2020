#include "uart.h"
#include "mystd.h"
// #include "exc.h"
// #include "irq.h"
// #include "task.h"
// #include "timer.h"

void main()
{
    uart_init();

    uart_puts("hello mmu!\n");
}
