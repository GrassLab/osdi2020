#include "kernel/peripherals/uart.h"

void exception_not_implemented ( unsigned long a )
{
    sys_printk ( "Exception Not Implemented!!\n" );
}