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
    // printDebugPowerOnMessage();
    // unsigned int * r=*((volatile unsigned int*)0xFFFFFFFFFF000000);
    asm volatile("svc #1");
    local_timer_init();
    sys_timer_init();
    interative();
}