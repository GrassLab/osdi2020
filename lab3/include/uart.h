#define MMIO_BASE       0x3F000000
/* Auxilary mini UART registers */
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
#define UART0_ICR       ((volatile unsigned int*)(MMIO_BASE+0x00201044))
#define IRQ2_EN         ((volatile unsigned int*)(MMIO_BASE+0x0000b214))
#define UART0_RIS       ((volatile unsigned int*)(MMIO_BASE+0x0020103c))
#define UART0_MIS       ((volatile unsigned int*)(MMIO_BASE+0x00201040))
#define UARTBUF_SIZE    1024

void mini_uart_init();
void PL011_uart_init();
void uart_puts(char *s);
void uart_hex(unsigned int d);
void uart_gets(char *s);
void uart_dec(unsigned long i);
void uart_double(double time);

extern void (*uart_send)(unsigned int );
extern char (*uart_getc)();

void enable_uart0_irq();

struct uart_buf{
    char buf[UARTBUF_SIZE];
    int head;
    int tail;
} read_buf, write_buf;

int queue_empty(struct uart_buf*);
int queue_full (struct uart_buf*);
void enqueue(struct uart_buf*, char);
char dequeue(struct uart_buf*);