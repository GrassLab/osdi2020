
#include "lib/io.h"
#include "lib/task.h"
#include "lib/time.h"

#include "shell.h"
#include "test_task.h"

int main ( )
{
    printf ( "This is main before test_task!!\n" );

    exec ( test_task );

    printf ( "This shall never be touched!!\n" );

    return 0;
}

#include "kernel/peripherals/uart.h"
int kernel_testing_main ( )
{
    sys_printk ( "Hello Wordl from EL1\n" );

    return 0;
}