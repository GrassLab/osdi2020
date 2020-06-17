#include "nlibc.h"

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
		uart_write("0x");
	}
	uart_write(ptr);
}

int printf(const char *fmt, ...)
{
	__builtin_va_list args;
	__builtin_va_start(args, fmt);
	while (*fmt != '\0') {
		if (*fmt == '%') {
			switch (*(++fmt)) {
			case '%':
				uart_write("%");
				break;
			case 's':
				//char *p = va_arg(args, char *);
				uart_write(__builtin_va_arg(args, char *));
				break;
			case 'd':
				//int arg = va_arg(args, int);
				num(__builtin_va_arg(args, unsigned int), 10);
				break;
			case 'x':
				//int arg = va_arg(args, int);
				num(__builtin_va_arg(args, unsigned int), 16);
				break;
			}
		} else {
			if (*fmt == '\n') {
				uart_write("\r");
			}
			char tmp[2] = { *fmt, '\0' };
			uart_write(tmp);
		}
		fmt++;
	}
	__builtin_va_end(args);
	return 1;
}