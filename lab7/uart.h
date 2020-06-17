#define MMIO_BASE 0x3F000000
#define UART0_DR ((volatile unsigned int *)(MMIO_BASE + 0x00201000))
#define UART0_FR ((volatile unsigned int *)(MMIO_BASE + 0x00201018))
#define UART0_IBRD ((volatile unsigned int *)(MMIO_BASE + 0x00201024))
#define UART0_FBRD ((volatile unsigned int *)(MMIO_BASE + 0x00201028))
#define UART0_LCRH ((volatile unsigned int *)(MMIO_BASE + 0x0020102C))
#define UART0_CR ((volatile unsigned int *)(MMIO_BASE + 0x00201030))
#define UART0_IMSC ((volatile unsigned int *)(MMIO_BASE + 0x00201038))
#define UART0_ICR ((volatile unsigned int *)(MMIO_BASE + 0x00201044))

char uart_img();
void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);
void uart_hex(unsigned int d);
void uart_hex_64(unsigned long long int d);
void wait_cycles(unsigned int n);
void printf(char *fmt, ...);
