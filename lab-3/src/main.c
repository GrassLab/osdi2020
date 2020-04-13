#include "uart.h"
#include "string.h"
#include "utility.h"
#include "mailbox.h"
#include "timer.h"
#include "command.h"

int main()
{
    uart_init();
    printPowerOnMessage();
    enable_irq();
    local_timer_init();
    core_timer_init();
    interative();
}
