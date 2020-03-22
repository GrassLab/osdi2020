#ifndef __GPIO_H__
#define __GPIO_H__

#define MMIO_BASE 0x3F000000

#define GPFSEL1         (MMIO_BASE + 0x00200004)
#define GPSET0          (MMIO_BASE + 0x0020001C)
#define GPCLR0          (MMIO_BASE + 0x00200028)
#define GPPUD           (MMIO_BASE + 0x00200094)
#define GPPUDCLK0       (MMIO_BASE + 0x00200098)

#endif
