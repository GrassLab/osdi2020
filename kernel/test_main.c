
#include "peripherals/uart.h"

int kernel_testing_main ( )
{
    sys_printk ( "Hello World from EL1\n" );

    return 0;
}