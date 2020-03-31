#ifndef	_UART_H
#define	_UART_H


#define UART_FR_CTS         (1<<0)// Clear to send
#define UART_FR_BUSY        (1<<3)// UART busy
#define UART_FR_RXFE        (1<<4)// Receive FIFO empty
#define UART_FR_TXFF        (1<<5)// Transmit FIFO full
#define UART_FR_RXFF        (1<<6)// Receive FIFO full
#define UART_FR_TXFE        (1<<6)// Transmit FIFO empty

#define UART_LCRH_BRK       (1<<0)// Send break
#define UART_LCRH_PEN       (1<<1)// Parity enable
#define UART_LCRH_EPS       (1<<2)// Even parity select
#define UART_LCRH_STP2      (1<<3)// Two stop bits selec
#define UART_LCRH_FEN       (1<<4)// Enable FIFOs
#define UART_LCRH_WLEN      (3<<5)// Word length
#define UART_LCRH_WLEN_5    (0b00<<5)// Word length 5 bits
#define UART_LCRH_WLEN_6    (0b01<<5)// Word length 6 bits
#define UART_LCRH_WLEN_7    (0b10<<5)// Word length 7 bits
#define UART_LCRH_WLEN_8    (0b11<<5)// Word length 8 bits
#define UART_LCRH_SPS       (1<<7)// Stick parity select

#define UART_CR_UARTEN      (1<<0)// UART enable
#define UART_CR_LBE         (1<<7)// Loopback enable
#define UART_CR_TXE         (1<<8)// Transmit enable
#define UART_CR_RXE         (1<<9)// Receive enable
#define UART_CR_RTS         (1<<11)// Request to send
#define UART_CR_RTSEN       (1<<14)// CTS hardware flow control enable
#define UART_CR_CTSEN       (1<<15)// RTS hardware flow control enable


void uart_init(void);
void uart_send(char c);
char uart_recv(void);
char uart_getc(void);
void uart_putc(char c);
void uart_puts(char *s);

#endif//_UART_H
