#ifndef __UART_H_
#define __UART_H_

#include "mm.h"

#define AUX_ENABLE          ((volatile unsigned int*)(MMIO_BASE+0x00215004))
#define AUX_MU_IO_REG       ((volatile unsigned int*)(MMIO_BASE+0x00215040))
#define AUX_MU_IER_REG      ((volatile unsigned int*)(MMIO_BASE+0x00215044))
#define AUX_MU_IIR_REG      ((volatile unsigned int*)(MMIO_BASE+0x00215048))
#define AUX_MU_LCR_REG      ((volatile unsigned int*)(MMIO_BASE+0x0021504C))
#define AUX_MU_MCR_REG      ((volatile unsigned int*)(MMIO_BASE+0x00215050))
#define AUX_MU_LSR_REG      ((volatile unsigned int*)(MMIO_BASE+0x00215054))
#define AUX_MU_MSR_REG      ((volatile unsigned int*)(MMIO_BASE+0x00215058))
#define AUX_MU_SCRATCH_REG  ((volatile unsigned int*)(MMIO_BASE+0x0021505C))
#define AUX_MU_CNTL_REG     ((volatile unsigned int*)(MMIO_BASE+0x00215060))
#define AUX_MU_STAT_REG     ((volatile unsigned int*)(MMIO_BASE+0x00215064))
#define AUX_MU_BAUD_REG     ((volatile unsigned int*)(MMIO_BASE+0x00215068))

void uart_setup();

uint8_t uart_getc();

void uart_putc(uint8_t c);

void uart_puts(const char* str);

void uart_read_line(char* buffer, size_t size);

#endif
