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
    // printPowerOnMessage();
    enable_irq();
    core_timer_init();
    task_manager_init();

    for(int i = 0; i < 3; ++i) {
        privilege_task_create(user_test);
    }
    // for(int i = 0; i < 3; ++i) {
    //     exec(foo);
    // }
    idle();
    interative();
}
