#ifndef __UART_H__
#define __UART_H__

#include "meta_macro.h"

/* !!! BCM2837 ARM Peripherals wrote 0x7E20100 missing one zero */
#define USRT_BASE PERIPHERAL_TO_PHYSICAL(0x7E201000)

#define UART_DR     ((uint32_t *)(USRT_BASE + 0x0 ))
#define UART_RSRECR ((uint32_t *)(USRT_BASE + 0x4 ))
#define UART_FR     ((uint32_t *)(USRT_BASE + 0x18))
#define UART_ILPR   ((uint32_t *)(USRT_BASE + 0x20))
#define UART_IBRD   ((uint32_t *)(USRT_BASE + 0x24))
#define UART_FBRD   ((uint32_t *)(USRT_BASE + 0x28))
#define UART_LCRH   ((uint32_t *)(USRT_BASE + 0x2c))
#define UART_CR     ((uint32_t *)(USRT_BASE + 0x30))
#define UART_IFLS   ((uint32_t *)(USRT_BASE + 0x34))
#define UART_IMSC   ((uint32_t *)(USRT_BASE + 0x38))
#define UART_RIS    ((uint32_t *)(USRT_BASE + 0x3c))
#define UART_MIS    ((uint32_t *)(USRT_BASE + 0x40))
#define UART_ICR    ((uint32_t *)(USRT_BASE + 0x44))
#define UART_DMACR  ((uint32_t *)(USRT_BASE + 0x48))
#define UART_ITCR   ((uint32_t *)(USRT_BASE + 0x80))
#define UART_ITIP   ((uint32_t *)(USRT_BASE + 0x84))
#define UART_ITOP   ((uint32_t *)(USRT_BASE + 0x88))
#define UART_TDR    ((uint32_t *)(USRT_BASE + 0x8c))

void uart_init(void);
char uart_getc(int echo);
char uart_putc(const char c);
void uart_puts(const char * string);
void uart_gets(char * string, char delimiter, unsigned length);
void uart_getn(char * buffer, unsigned length);

#endif

