#ifndef UART
#define UART

#include <mm.h>

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
#define UART0_DR        ((volatile unsigned int*)(MMIO_BASE+0x00201000))
#define UART0_FR        ((volatile unsigned int*)(MMIO_BASE+0x00201018))
#define UART0_IBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201024))
#define UART0_FBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201028))
#define UART0_LCRH      ((volatile unsigned int*)(MMIO_BASE+0x0020102C))
#define UART0_CR        ((volatile unsigned int*)(MMIO_BASE+0x00201030))
#define UART0_IMSC      ((volatile unsigned int*)(MMIO_BASE+0x00201038))
#define UART0_RIS       ((volatile unsigned int*)(MMIO_BASE+0x0020103c))
#define UART0_ICR       ((volatile unsigned int*)(MMIO_BASE+0x00201044))

#define UARTBUF_SIZE 0x30
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

void uart_init ();
void uart_send (char c);
void uart_puts (char *s);
char uart_getc () __attribute__ ((section (".bootloader")));
int uart_readline (int size, char *buf);
void uart_hex (unsigned int d);
void uart_read (char *buf, unsigned long count)
  __attribute__ ((section (".bootloader")));
#endif /* ifndef UART */
