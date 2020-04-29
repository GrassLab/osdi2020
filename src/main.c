#include "kernel/peripherals/uart.h"
#include "lib/task.h"

#include "shell.h"
#include "test_task.h"


int main ( )
{
    // set up serial console
    uart_init();

    // say hello
    uart_printf("Hello World\n");

    // start shell
    // shell_start();

    // test do_exec, it shall just jump to that process
    do_exec ( test_task );
    
    return 0;
}