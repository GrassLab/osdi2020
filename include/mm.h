/*
 * mm.h
 * Provides a basic API to interact with rpi3's GPIOs
 * 
 * source: https://jsandler18.github.io/tutorial/boot.html
 */

#ifndef __MM_H_
#define __MM_H_

enum {
    MMIO_BASE       = 0x3F000000,
    MAILBOX_BASE    = (MMIO_BASE + 0xb880),
    MAILBOX_READ    = (MAILBOX_BASE + 0x0),
    MAILBOX_STATUS  = (MAILBOX_BASE + 0x18),
    MAILBOX_WRITE   = (MAILBOX_BASE + 0x20),

    MAILBOX_EMPTY   = 0x40000000,
    MAILBOX_FULL    = 0x80000000,

    GPFSEL1         = (MMIO_BASE+0x00200004),
    GPSET0          = (MMIO_BASE+0x0020001C),
    GPCLR0          = (MMIO_BASE+0x00200028),
    GPPUD           = (MMIO_BASE+0x00200094),
    GPPUDCLK0       = (MMIO_BASE+0x00200098),

    HCR_IMO         = (1 << 4),
    HCR_RW          = (1 << 31)
};

#include "types.h"

static inline void mm_write(uint32_t reg, uint32_t data) {
    *(volatile unsigned int*)reg = data;
}

static inline uint32_t mm_read(uint32_t reg) {
    return *(volatile unsigned int*)reg;
}

#endif
