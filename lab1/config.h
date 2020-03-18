#include "uart.h"
#include "string.h"
#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int*)(0x3F10001c))
#define PM_WDOG ((volatile unsigned int*)(0x3F100024))
#define CMD_NUM 4

