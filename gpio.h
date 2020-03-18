#ifndef	_P_GPIO_H
#define	_P_GPIO_H

#define MMIO_BASE       0x3F000000

#define GPFSEL1         ((volatile unsigned int*)(MMIO_BASE+0x00200004))
#define GPPUD           ((volatile unsigned int*)(MMIO_BASE+0x00200094))
#define GPPUDCLK0       ((volatile unsigned int*)(MMIO_BASE+0x00200098))


#endif  /*_P_GPIO_H */