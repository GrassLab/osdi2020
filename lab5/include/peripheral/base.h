#ifndef __PERIPHERAL_BASE_H
#define __PERIPHERAL_BASE_H

// check section 1.2.3 of BCM2837 ARM Peripherals manual
#define VIRTUAL_ADDRESS_BASE 0xffff000000000000
// #define VIRTUAL_ADDRESS_BASE 0
#define MMIO_BASE (VIRTUAL_ADDRESS_BASE + 0x3F000000)

#endif
