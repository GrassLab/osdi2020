#ifndef	_UART_H
#define	_UART_H

// http://infocenter.arm.com/help/topic/com.arm.doc.ddi0183g/DDI0183G_uart_pl011_r1p5_trm.pdf
//  Separate 32×8 transmit and 32×12 receive First-In, First-Out (FIFO) memory buffers to reduce CPU interrupts.

typedef struct {
    volatile unsigned int DR;        // (+0x00) Data Register
    volatile unsigned int RSRECR;    // (+0x04) Receive data status register
    volatile unsigned int UNKNOWN0;  // (+0x08)
    volatile unsigned int UNKNOWN1;  // (+0x0C)
    volatile unsigned int UNKNOWN2;  // (+0x10)
    volatile unsigned int UNKNOWN3;  // (+0x14)
    volatile unsigned int FR;        // (+0x18) Flag register
    volatile unsigned int UNKNOWN4;  // (+0x1c)
    volatile unsigned int ILPR;      // (+0x20) not in use
    volatile unsigned int IBRD;      // (+0x24) Integer Baud rate divisor
    volatile unsigned int FBRD;      // (+0x28) Fractional Baud rate divisor
    volatile unsigned int LCRH;      // (+0x2C) Line Control register
    volatile unsigned int CR;        // (+0x30) Control register
    volatile unsigned int IFLS;      // (+0x34) Interupt FIFO Level Select Register
    volatile unsigned int IMSC;      // (+0x38) Interupt Mask Set Clear Register
    volatile unsigned int RIS;       // (+0x3c) Raw Interupt Status Register
    volatile unsigned int MIS;       // (+0x40) Masked Interupt Status Register
    volatile unsigned int ICR;       // (+0x44) Interupt Clear Register
    volatile unsigned int DMACR;     // (+0x48) DMA Control Register
} UART_Typedef;
#define UART_REG ((UART_Typedef *)UART_REG_BASE)


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
#define UART_LCRH_WLEN_5    (0<<5)// Word length 5 bits
#define UART_LCRH_WLEN_6    (1<<5)// Word length 6 bits
#define UART_LCRH_WLEN_7    (2<<5)// Word length 7 bits
#define UART_LCRH_WLEN_8    (3<<5)// Word length 8 bits
#define UART_LCRH_SPS       (1<<7)// Stick parity select

#define UART_CR_UARTEN      (1<<0)// UART enable
#define UART_CR_LBE         (1<<7)// Loopback enable
#define UART_CR_TXE         (1<<8)// Transmit enable
#define UART_CR_RXE         (1<<9)// Receive enable
#define UART_CR_RTS         (1<<11)// Request to send
#define UART_CR_RTSEN       (1<<14)// CTS hardware flow control enable
#define UART_CR_CTSEN       (1<<15)// RTS hardware flow control enable

#define UART_IFLS_RXIFLSEL_1_8_FULL  (0b000)
#define UART_IFLS_RXIFLSEL_1_4_FULL  (0b001)
#define UART_IFLS_RXIFLSEL_1_2_FULL  (0b010)
#define UART_IFLS_RXIFLSEL_3_4_FULL  (0b011)
#define UART_IFLS_RXIFLSEL_7_8_FULL  (0b100)

#define UART_IFLS_TXIFLSEL_1_8_FULL  (0b000)
#define UART_IFLS_TXIFLSEL_1_4_FULL  (0b001)
#define UART_IFLS_TXIFLSEL_1_2_FULL  (0b010)
#define UART_IFLS_TXIFLSEL_3_4_FULL  (0b011)
#define UART_IFLS_TXIFLSEL_7_8_FULL  (0b100)

#define UART_IMSC_RXIM      (1<<4)// Receive interrupt mask
#define UART_IMSC_TXIM      (1<<5)// Transmit interrupt mask
#define UART_IMSC_RTIM      (1<<6)// Receive timeout interrupt mask
#define UART_IMSC_FEIM      (1<<7)// Framing error interrupt mask
#define UART_IMSC_PEIM      (1<<8)// Parity error interrupt mask
#define UART_IMSC_BEIM      (1<<9)// Break error interrupt mask
#define UART_IMSC_OEIM      (1<<10)// Overrun error interrupt mask

#define UART_ICR_RXIC       (1<<4)// Receive interrupt clear 
#define UART_ICR_TXIC       (1<<5)// Transmit interrupt clear
#define UART_ICR_RTIC       (1<<6)// Receive timeout interrupt clear
#define UART_ICR_FEIC       (1<<7)// Framing error interrupt clear
#define UART_ICR_PEIC       (1<<8)// Parity error interrupt clear
#define UART_ICR_BEIC       (1<<9)// Break error interrupt clear
#define UART_ICR_OEIC       (1<<10)// Overrun error interrupt clear


void uart_init(void);
void uart_send(char c);
char uart_recv(void);
char uart_getc(void);
void uart_putc(void* p, char c);
void uart_puts(char *s);


void uart_hex(unsigned int d);

#endif//_UART_H
