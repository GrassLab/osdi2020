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

    // for(int i = 0; i < 4; ++i) {
    //     privilege_task_create(req12_test);
    // }

    privilege_task_create(req34_test);
    enable_irq();
    core_timer_init();


    // idle12();
    idle();
}
