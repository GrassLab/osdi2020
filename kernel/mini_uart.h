#ifndef KERNEL_MINI_UART_H_
#define KERNEL_MINI_UART_H_

#include "kernel/base.h"
#include "kernel/lib/types.h"
#include "kernel/lock.h"

#define GPFSEL1   ((volatile uint32_t *)(GPU_PERIPHERAL_BASE + 0x200004))
#define GPPUD     ((volatile uint32_t *)(GPU_PERIPHERAL_BASE + 0x200094))
#define GPPUDCLK0 ((volatile uint32_t *)(GPU_PERIPHERAL_BASE + 0x200098))

#define AUXENB          ((volatile uint8_t *)(GPU_PERIPHERAL_BASE + 0x215004))
#define AUX_MU_IO_REG   ((volatile uint8_t *)(GPU_PERIPHERAL_BASE + 0x215040))
#define AUX_MU_IER_REG  ((volatile uint8_t *)(GPU_PERIPHERAL_BASE + 0x215044))
#define AUX_MU_IIR_REG  ((volatile uint8_t *)(GPU_PERIPHERAL_BASE + 0x215048))
#define AUX_MU_LCR_REG  ((volatile uint8_t *)(GPU_PERIPHERAL_BASE + 0x21504c))
#define AUX_MU_MCR_REG  ((volatile uint8_t *)(GPU_PERIPHERAL_BASE + 0x215050))
#define AUX_MU_LSR_REG  ((volatile uint8_t *)(GPU_PERIPHERAL_BASE + 0x215054))
#define AUX_MU_CNTL_REG ((volatile uint8_t *)(GPU_PERIPHERAL_BASE + 0x215060))
#define AUX_MU_BAUD     ((volatile uint16_t *)(GPU_PERIPHERAL_BASE + 0x215068))

struct mutex uart_lock;

void gpio_init(void);
void mini_uart_init(void);
uint8_t mini_uart_getc(bool verbose);
void mini_uart_getn(bool verbose, uint8_t *buf, uint32_t n);
void mini_uart_gets(char *buf);
void mini_uart_putc(uint8_t c);
void mini_uart_putn(const uint8_t *buf, uint32_t n);
void mini_uart_puts(const uint8_t *buf);

#endif // KERNEL_MINI_UART_H_
