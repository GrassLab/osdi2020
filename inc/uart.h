#ifndef UART_H
#define UART_H

/* Auxilary mini UART registers */
#define AUX_ENABLE ((volatile unsigned int*)(MMIO_BASE + 0x00215004))
#define AUX_MU_IO ((volatile unsigned int*)(MMIO_BASE + 0x00215040))
#define AUX_MU_IER ((volatile unsigned int*)(MMIO_BASE + 0x00215044))
#define AUX_MU_IIR ((volatile unsigned int*)(MMIO_BASE + 0x00215048))
#define AUX_MU_LCR ((volatile unsigned int*)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR ((volatile unsigned int*)(MMIO_BASE + 0x00215050))
#define AUX_MU_LSR ((volatile unsigned int*)(MMIO_BASE + 0x00215054))
#define AUX_MU_MSR ((volatile unsigned int*)(MMIO_BASE + 0x00215058))
#define AUX_MU_SCRATCH ((volatile unsigned int*)(MMIO_BASE + 0x0021505C))
#define AUX_MU_CNTL ((volatile unsigned int*)(MMIO_BASE + 0x00215060))
#define AUX_MU_STAT ((volatile unsigned int*)(MMIO_BASE + 0x00215064))
#define AUX_MU_BAUD ((volatile unsigned int*)(MMIO_BASE + 0x00215068))

#define IER_REG_EN_REC_INT (1 << 0)
#define IER_REG_INT (3 << 2)  // Must be set to receive interrupts
#define IER_REG_VALUE (IER_REG_EN_REC_INT | IER_REG_INT)

#define BUFFER_MAX 200

void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(const char *s);
void uart_hex(unsigned int d);

#endif 
