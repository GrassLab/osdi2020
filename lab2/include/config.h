#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include "uart.h"
#include "string.h"
#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int*)(0x3F10001c))
#define PM_WDOG ((volatile unsigned int*)(0x3F100024))
#define CMD_NUM 5
typedef unsigned char           uint8_t;



#endif 