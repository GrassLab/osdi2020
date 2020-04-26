#include "kernel/peripherals/uart.h"
#include "kernel/task/task.h"

#include "shell.h"
#include "test_task.h"

int el1_main ()
{
    uart_init ( );

    task_schedule_test ();

    return 0;
}

int main()
{
    // set up serial console
    uart_init();

    // say hello
    uart_printf("Hello World\n");

    // start shell
    shell_start();
    
    return 0;
}