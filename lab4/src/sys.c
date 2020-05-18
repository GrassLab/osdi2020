#include "shed.h"
#include "mm.h"
#include "uart.h"

void do_uart_write(char *arg) {
	uart_puts(arg);
}

void do_uart_read(char *line) {
	uart_read_line(line);
}

void do_exec(void(*func)()) {
    _do_exec(func);
}

int do_fork() {
	int f = _do_fork();
	uart_puts("[In do_fork()] ret: ");
	uart_print_int(f);
	uart_puts("\r\n");
	return f;
}

void do_exit() {
	_do_exit();
}

void * const sys_call_table[] = {do_uart_write, do_uart_read, do_exec, do_fork, do_exit};