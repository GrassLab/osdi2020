#include "stdint.h"

#ifndef __SYS_H__
#define __SYS_H__

#define SYS_GET_CNTFRQ      0
#define SYS_GET_CNTPCT      1
#define SYS_UART_READ       2
#define SYS_UART_WRITE      3

#endif

uint64_t sys_get_cntfrq();
uint64_t sys_get_cntpct();
uint32_t sys_uart_read(char buf[], uint32_t size);
uint32_t sys_uart_write(const char buf[], uint32_t size);
