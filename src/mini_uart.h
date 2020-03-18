#ifndef MINI_UART_H_
#define MINI_UART_H_

#include "types.h"

#define GPFSEL1 ((uint32_t *)0x3f200004)
#define GPPUD ((uint32_t *)0x3f200094)
#define GPPUDCLK0 ((uint32_t *)0x3f200098)

#define AUXENB ((uint8_t *)0x3f215004)
#define AUX_MU_IO_REG ((uint8_t *)0x3f215040)
#define AUX_MU_IER_REG ((uint8_t *)0x3f215044)
#define AUX_MU_IIR_REG ((uint8_t *)0x3f215048)
#define AUX_MU_LCR_REG ((uint8_t *)0x3f21504c)
#define AUX_MU_MCR_REG ((uint8_t *)0x3f215050)
#define AUX_MU_LSR_REG ((uint8_t *)0x3f215054)
#define AUX_MU_CNTL_REG ((uint8_t *)0x3f215060)
#define AUX_MU_BAUD ((uint16_t *)0x3f215068)

void gpio_init(void);
void mini_uart_init(void);
uint8_t mini_uart_getc(void);
void mini_uart_gets(char *buf);
void mini_uart_putc(uint8_t c);
void mini_uart_puts(uint8_t *s);

#endif // MINI_UART_H_
