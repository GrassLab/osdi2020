#include "kernel/peripherals/uart.h"
#include "kernel/task/task.h"

#include "shell.h"
#include "test_task.h"

int el1_main ()
{
    uart_init ( );

    uart_printf("Hello World!!\n");

    privilege_task_create ( task_1 );
    privilege_task_create ( task_2 );    

    task_switch_test ();

    // shell_start();

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