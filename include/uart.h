#ifndef __UART_H_
#define __UART_H_

#include "mm.h"

enum {
    AUX_ENABLE          = (MMIO_BASE+0x00215004),
    AUX_MU_IO_REG       = (MMIO_BASE+0x00215040),
    AUX_MU_IER_REG      = (MMIO_BASE+0x00215044),
    AUX_MU_IIR_REG      = (MMIO_BASE+0x00215048),
    AUX_MU_LCR_REG      = (MMIO_BASE+0x0021504C),
    AUX_MU_MCR_REG      = (MMIO_BASE+0x00215050),
    AUX_MU_LSR_REG      = (MMIO_BASE+0x00215054),
    AUX_MU_MSR_REG      = (MMIO_BASE+0x00215058),
    AUX_MU_SCRATCH_REG  = (MMIO_BASE+0x0021505C),
    AUX_MU_CNTL_REG     = (MMIO_BASE+0x00215060),
    AUX_MU_STAT_REG     = (MMIO_BASE+0x00215064),
    AUX_MU_BAUD_REG     = (MMIO_BASE+0x00215068)
};

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} log_level_t;

void uart_setup();

uint8_t uart_getc();

void uart_putc(uint8_t c);

void uart_puts(const char* str);

void uart_hex(unsigned int d);

void uart_log(log_level_t level, const char *message);

#endif
