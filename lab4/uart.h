#include "gpio.h"


#define UART0_DR        ((volatile unsigned int*)(MMIO_BASE+0x00201000))
#define UART0_FR        ((volatile unsigned int*)(MMIO_BASE+0x00201018))
#define UART0_IBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201024))
#define UART0_FBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201028))
#define UART0_LCRH      ((volatile unsigned int*)(MMIO_BASE+0x0020102C))
#define UART0_CR        ((volatile unsigned int*)(MMIO_BASE+0x00201030))
#define UART0_IMSC      ((volatile unsigned int*)(MMIO_BASE+0x00201038))
#define UART0_ICR       ((volatile unsigned int*)(MMIO_BASE+0x00201044))
#define ENABLE_IRQ2     ((volatile unsigned int*)(MMIO_BASE+0x0000b214))
#define UART0_RIS       ((volatile unsigned int*)(MMIO_BASE+0x0020103C))
#define UART0_IFLS      ((volatile unsigned int*)(MMIO_BASE+0x00201034))
#define TMP_SOR ((volatile unsigned int*)0x20000)


void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);
void uart_hex(unsigned int d);

struct myqueue
{
    int head,tail;
    unsigned int buf[0x100];
}rec_buf, tran_buf;