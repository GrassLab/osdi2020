#include "uart.h"
#include "string.h"
#include "utility.h"
#include "mailbox.h"
#include "time.h"
#include "command.h"
#include "task.h"

int main()
{
    uart_init();
    printPowerOnMessage();
    task_manager_init(idle);
    uart_puts("OK0\n");

    for(int i = 0; i < 4; ++i) {
        privilege_task_create(req12_test);
    }

    // privilege_task_create(req34_test);
    uart_puts("OK1\n");
    __enable_irq();
    __core_timer_init();
    uart_puts("OK2\n");

    // idle12();
    idle();
}
