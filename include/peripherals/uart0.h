#include "peripherals/mmio.h"

#ifndef UART0_H
#define UART0_H

#define UART0_BASE      (MMIO_BASE + 0x201000)

#define UART0_DR        ((unsigned int*)(UART0_BASE))
#define UART0_FR        ((unsigned int*)(UART0_BASE + 0x18))
#define UART0_IBRD      ((unsigned int*)(UART0_BASE + 0x24))
#define UART0_FBRD      ((unsigned int*)(UART0_BASE + 0x28))
#define UART0_LCRH      ((unsigned int*)(UART0_BASE + 0x2C))
#define UART0_CR        ((unsigned int*)(UART0_BASE + 0x30))
#define UART0_IMSC      ((unsigned int*)(UART0_BASE + 0x38))
#define UART0_RIS       ((unsigned int*)(UART0_BASE + 0x3C))
#define UART0_MIS       ((unsigned int*)(UART0_BASE + 0x40))
#define UART0_ICR       ((unsigned int*)(UART0_BASE + 0x44))

#endif