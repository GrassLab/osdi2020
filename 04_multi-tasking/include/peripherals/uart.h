#ifndef _P_UART_H
#define _P_UART_H

#include "peripherals/base.h"

#define AUX_ENABLES (PBASE + 0x00215004)
#define AUX_MU_IO_REG (PBASE + 0x00215040)
#define AUX_MU_IER_REG (PBASE + 0x00215044)
#define AUX_MU_IIR_REG (PBASE + 0x00215048)
#define AUX_MU_LCR_REG (PBASE + 0x0021504C)
#define AUX_MU_MCR_REG (PBASE + 0x00215050)
#define AUX_MU_LSR_REG (PBASE + 0x00215054)
#define AUX_MU_MSR_REG (PBASE + 0x00215058)
#define AUX_MU_SCRATCH (PBASE + 0x0021505C)
#define AUX_MU_CNTL_REG (PBASE + 0x00215060)
#define AUX_MU_STAT_REG (PBASE + 0x00215064)
#define AUX_MU_BAUD_REG (PBASE + 0x00215068)

void putc(void *p, char c);
void uart_init(void);
char uart_recv(void);
void uart_send(char c);
void uart_send_string(const char *str);
void putc(void *p, char c);
int readline(char *buf, int maxlen);
void uart_send_int(int number);
void uart_send_hex(unsigned long number);
void handle_uart_irq();

#endif
