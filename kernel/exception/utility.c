#include "kernel/peripherals/uart.h"

void exception_not_implemented (  unsigned long a  )
{
    uart_printf("Exception Not Implemented!!\n");
}