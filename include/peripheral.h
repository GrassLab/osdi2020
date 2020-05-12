#include "gpio.h"

/* PL011 UART registers */
#define UART0_DR        ((volatile unsigned int*)(MMIO_BASE+0x00201000)) // 0x??201000 data register
#define UART0_FR        ((volatile unsigned int*)(MMIO_BASE+0x00201018)) // 0x??201018 flag register
#define UART0_IBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201024)) // 0x??201024 Integer Baud rate divisor
#define UART0_FBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201028)) // 0x??201028 Fractional Baud rate divisor
#define UART0_LCRH      ((volatile unsigned int*)(MMIO_BASE+0x0020102C)) // 0x??20102C Line Control register
#define UART0_CR        ((volatile unsigned int*)(MMIO_BASE+0x00201030)) // 0x??201030 Control register
#define UART0_IMSC      ((volatile unsigned int*)(MMIO_BASE+0x00201038)) // 0x??201038 Interupt Mask Set Clear Register
#define UART0_RIS       ((volatile unsigned int*)(MMIO_BASE+0x0020103c)) // 0x??20103C Raw Interupt Status Register
#define UART0_ICR       ((volatile unsigned int*)(MMIO_BASE+0x00201044)) // 0x??201044 Interupt Clear Register



#define TX_FIFO_FULL    (*UART0_FR & (1<<5))
#define TX_FIFO_EMPTY   (*UART0_FR & (1<<7))
#define RX_FIFO_FULL    (*UART0_FR & (1<<6))
#define RX_FIFO_EMPTY   (*UART0_FR & (1<<4))



char TX_BUF[0x10];
char RX_BUF[0x10];
