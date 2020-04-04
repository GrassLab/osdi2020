#include "utils.h"
#include "mini_uart.h"
#include "entry.h"
#include "sys.h"

void handle_sync(unsigned long esr, unsigned long address)
{
    uart_send_hex(address);
    uart_send_hex(esr>>26);
    uart_send_hex(esr & 0xfff);
}
