#ifndef SHELL_H_
#define SHELL_H_

#include "mini_uart.h"

#define MAX_CMD_LEN 1024
#define MAX_TIMER_FRACTION_LEN 6

extern char __text_start[];
extern char __text_end[];

#define PM_RSTC ((uint32_t *)0x3f10001c)
#define PM_WDOG ((uint32_t *)0x3f100024)
#define PM_PASSWORD 0x5a000000

void help(void);
void hello(void);
void timestamp(void);
void reboot(void);

void shell(void);

#endif // SHELL_H_
