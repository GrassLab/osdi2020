#include "gpio.h"

/* Mini UART registers */
#define AUX_ENABLE      ((volatile unsigned int*)(MMIO_BASE+0x00215004))
#define AUX_MU_IO       ((volatile unsigned int*)(MMIO_BASE+0x00215040))
#define AUX_MU_IER      ((volatile unsigned int*)(MMIO_BASE+0x00215044))
#define AUX_MU_IIR      ((volatile unsigned int*)(MMIO_BASE+0x00215048))
#define AUX_MU_LCR      ((volatile unsigned int*)(MMIO_BASE+0x0021504C))
#define AUX_MU_MCR      ((volatile unsigned int*)(MMIO_BASE+0x00215050))
#define AUX_MU_LSR      ((volatile unsigned int*)(MMIO_BASE+0x00215054))
#define AUX_MU_MSR      ((volatile unsigned int*)(MMIO_BASE+0x00215058))
#define AUX_MU_SCRATCH  ((volatile unsigned int*)(MMIO_BASE+0x0021505C))
#define AUX_MU_CNTL     ((volatile unsigned int*)(MMIO_BASE+0x00215060))
#define AUX_MU_STAT     ((volatile unsigned int*)(MMIO_BASE+0x00215064))
#define AUX_MU_BAUD     ((volatile unsigned int*)(MMIO_BASE+0x00215068))

/* PL011 UART registers */
#define UART0_DR        ((volatile unsigned int*)(MMIO_BASE+0x00201000)) // data register
#define UART0_FR        ((volatile unsigned int*)(MMIO_BASE+0x00201018)) // flag register
#define UART0_IBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201024)) // Integer Baud rate divisor
#define UART0_FBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201028)) // Fractional Baud rate divisor
#define UART0_LCRH      ((volatile unsigned int*)(MMIO_BASE+0x0020102C)) // Line Control register
#define UART0_CR        ((volatile unsigned int*)(MMIO_BASE+0x00201030)) // Control register
#define UART0_IMSC      ((volatile unsigned int*)(MMIO_BASE+0x00201038)) // Interupt Mask Set Clear Register
#define UART0_RIS       ((volatile unsigned int*)(MMIO_BASE+0x0020103c)) // Raw Interupt Status Register
#define UART0_ICR       ((volatile unsigned int*)(MMIO_BASE+0x00201044)) // Interupt Clear Register

#define UARTBUF_SIZE 0x100
#define QUEUE_EMPTY(q) (q.tail == q.head)
#define QUEUE_FULL(q) ((q.tail + 1) % UARTBUF_SIZE == q.head)
#define QUEUE_POP(q) (q.head = (q.head + 1) % UARTBUF_SIZE)
#define QUEUE_PUSH(q) (q.tail = (q.tail + 1) % UARTBUF_SIZE)
#define QUEUE_GET(q) (q.buf[q.head])
#define QUEUE_SET(q, val) (q.buf[q.tail] = val)

struct uart_buf
{
  int head;
  int tail;
  char buf[UARTBUF_SIZE];
} read_buf, write_buf;

void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);
void uart_hex(unsigned int d);
