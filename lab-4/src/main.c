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
    
    taskManagerInit();

    for(int i = 0; i < 10; ++i) {
        privilege_task_create(foo);
    }

    idle();
    
    interative();
}
