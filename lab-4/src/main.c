#include "uart.h"
#include "string.h"
#include "utility.h"
#include "mailbox.h"
#include "timer.h"
#include "command.h"
#include "task.h"

int main()
{
    uart_init();
    printPowerOnMessage();
    getTimestamp();
    interative();
}
