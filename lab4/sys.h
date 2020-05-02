#include <stdint.h>
#ifndef __SYS_H__
#define __SYS_H__

#define SYS_UART_PUTS 3
#define SYS_UART_GETS 4

int sys_exc(uint64_t ELR_EL1, uint8_t exception_class, uint32_t exception_iss);
int sys_timer_int(void);
int sys_uart_puts(char * string);
int sys_uart_gets(char * string, char delimiter, unsigned length);

#endif

