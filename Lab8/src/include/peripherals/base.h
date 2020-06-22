#ifndef	_P_BASE_H
#define	_P_BASE_H

#define VA_START  0xffff000000000000
#define DEVICE_BASE 0x3F000000
#define PBASE   (VA_START + DEVICE_BASE)
#define MMIO_BASE PBASE

#endif  /*_P_BASE_H */
