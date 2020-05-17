#ifndef	_GPIO_H
#define	_GPIO_H

#include "base.h"

#define GPFSEL1         ((volatile unsigned int*)(MMIO_BASE+0x00200004))
#define GPPUD           ((volatile unsigned int*)(MMIO_BASE+0x00200094))
#define GPPUDCLK0       ((volatile unsigned int*)(MMIO_BASE+0x00200098))


#endif  /*_GPIO_H */