#include "uart.h"
#include "string.h"
#include "utility.h"
#include "mailbox.h"
#include "command.h"

int main()
{
    printPowerOnMessage();
    // printDebugPowerOnMessage();
    uart_init();
    processCommand();
}