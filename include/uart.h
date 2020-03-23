#ifndef __MINI_UART_H__
#define __MINI_UART_H__

#include "gpio.h"

#ifdef MINIUART
#define UART_TYPE "MINI"
#define AUXENB          (MMIO_BASE + 0x00215004)
#define AUX_MU_CNTL_REG (MMIO_BASE + 0x00215060)
#define AUX_MU_IER_REG  (MMIO_BASE + 0x00215044)
#define AUX_MU_LCR_REG  (MMIO_BASE + 0x0021504C)
#define AUX_MU_MCR_REG  (MMIO_BASE + 0x00215050)
#define AUX_MU_BAUD_REG (MMIO_BASE + 0x00215068)
#define AUX_MU_IIR_REG  (MMIO_BASE + 0x00215048)
#define AUX_MU_CNTL_REG (MMIO_BASE + 0x00215060)
#define AUX_MU_IO_REG   (MMIO_BASE + 0x00215040)
#define AUX_MU_LSR_REG  (MMIO_BASE + 0x00215054)
#else
#define UART_TYPE "PL011"
#define UART_DR        (MMIO_BASE+0x00201000)
#define UART_FR        (MMIO_BASE+0x00201018)
#define UART_IBRD      (MMIO_BASE+0x00201024)
#define UART_FBRD      (MMIO_BASE+0x00201028)
#define UART_LCRH      (MMIO_BASE+0x0020102C)
#define UART_CR        (MMIO_BASE+0x00201030)
#define UART_IMSC      (MMIO_BASE+0x00201038)
#define UART_ICR       (MMIO_BASE+0x00201044)
#endif

void uart_send(char c);
char uart_recv(void);
void uart_flush(void);
void uart_send_string(char*);
void uart_init(void);

#endif
