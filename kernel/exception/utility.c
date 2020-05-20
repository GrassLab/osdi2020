#include "kernel/peripherals/uart.h"

void exception_not_implemented ( )
{
    sys_printk ( "Exception Not Implemented!!\n" );
}