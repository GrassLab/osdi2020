#include "uart.h"
// #include "vsprintf.h"
#include "stdint.h"

void num(unsigned int num, int base)
{
	static char Representation[] = "0123456789ABCDEF";
	static char buffer[50];
	char *ptr;
	ptr = &buffer[49];
	*ptr = '\0';
	do {
		*--ptr = Representation[num % base];
		num /= base;
	} while (num != 0);
	if (base == 16) {
		uart_puts("0x");
	}
	uart_puts(ptr);
}

int printk(const char *fmt, ...)
{
	// char buf[1024];
	// __builtin_va_list args;
	// __builtin_va_start(args, fmt);
	// uart_puts(vsprintf(buf, fmt, args));
	// __builtin_va_end(args);
	__builtin_va_list args;
	__builtin_va_start(args, fmt);
	while (*fmt != '\0') {
		if (*fmt == '%') {
			switch (*(++fmt)) {
			case '%':
				send('%');
				break;
			case 's':
				//char *p = va_arg(args, char *);
				uart_puts(__builtin_va_arg(args, char *));
				break;
			case 'd':
				//int arg = va_arg(args, int);
				num(__builtin_va_arg(args, uint32_t), 10);
				break;
			case 'x':
				//int arg = va_arg(args, int);
				num(__builtin_va_arg(args, uint32_t), 16);
				break;
			}
		} else {
			if (*fmt == '\n') {
				send('\r');
			}
			send(*fmt);
		}
		fmt++;
	}
	__builtin_va_end(args);
	return 1;
}

int print(const char *fmt, ...)
{
	__builtin_va_list args;
	__builtin_va_start(args, fmt);
	while (*fmt != '\0') {
		if (*fmt == '%') {
			switch (*(++fmt)) {
			case '%':
				send('%');
				break;
			case 's':
				//char *p = va_arg(args, char *);
				uart_puts(__builtin_va_arg(args, char *));
				break;
			case 'd':
				//int arg = va_arg(args, int);
				num(__builtin_va_arg(args, uint32_t), 10);
				break;
			case 'x':
				//int arg = va_arg(args, int);
				num(__builtin_va_arg(args, uint32_t), 16);
				break;
			}
		} else {
			if (*fmt == '\n') {
				send('\r');
			}
			send(*fmt);
		}
		fmt++;
	}
	__builtin_va_end(args);
	return 1;
}