#include "uart.h"
#include "vsprintf.h"

void printk(const char *fmt, ...)
{
	char buf[1024];
	__builtin_va_list args;
	__builtin_va_start(args, fmt);
	uart_puts(vsprintf(buf, fmt, args));
	__builtin_va_end(args);
}