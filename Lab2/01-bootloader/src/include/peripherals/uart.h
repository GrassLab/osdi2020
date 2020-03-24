#ifndef _P_UART_H
#define _P_UART_H

#include "base.h"

/* PL011 UART registers */
#define UART0_DR        (PBASE+0x00201000) //data register
#define UART0_FR        (PBASE+0x00201018) //flag register
#define UART0_IBRD      (PBASE+0x00201024) //Interger Baud rate divisor
#define UART0_FBRD      (PBASE+0x00201028) //Fractional Baud rate divisor
#define UART0_LCRH      (PBASE+0x0020102C) //Line Control register
#define UART0_CR        (PBASE+0x00201030) //Control register
#define UART0_IMSC      (PBASE+0x00201038) //Interrupt Mask set Clear Register
#define UART0_ICR       (PBASE+0x00201044) //Interrupt Clear register:

#endif /*P_UART_H */
