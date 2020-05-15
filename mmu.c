/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "gpio.h" // get MMIO_BASE
#include "uart.h"

#include "printf.h"

#define PAGESIZE 4096
#define PAGE (PAGESIZE / 8)

// granularity
#define PT_PAGE 0b11  // 4k granule
#define PT_BLOCK 0b01 // 2M granule

#define PT_PGD 0b11 // 2M granule
#define PT_PUD 0b01 // 2M granule
#define PT_PMD 0b01 // 2M granule
#define PT_PTE 0b11 // 2M granule
// accessibility
#define PT_KERNEL (0 << 6) // privileged, supervisor EL1 access only
#define PT_USER (1 << 6)   // unprivileged, EL0 access allowed
#define PT_RW (0 << 7)     // read-write
#define PT_RO (1 << 7)     // read-only
#define PT_AF (1 << 10)    // accessed flag
#define PT_NX (1UL << 54)  // no execute
// shareability
#define PT_OSH (2 << 8) // outter shareable
#define PT_ISH (3 << 8) // inner shareable
// defined in MAIR register
#define PT_MEM (0 << 2) // normal memory
#define PT_DEV (1 << 2) // device MMIO
#define PT_NC (2 << 2)  // non-cachable

#define TTBR_CNP 1

// get addresses from linker
extern volatile unsigned char _data;
extern volatile unsigned char _end;

/*
typedef struct page_t
{
    char p[512];
} page_t;
*/

/**
 * Set up page translation tables and enable virtual memory
 */
void mmu_init()
{
    unsigned long data_page = (unsigned long)&_data / PAGESIZE;
    unsigned long r, b, *paging = (unsigned long *)&_end;
    //unsigned long r, b, *paging = (unsigned long *)0;

    // Require 1-1: Set up TCR_EL1
    // next, specify mapping characteristics in translate control register
    // Lower VA subrange ： 0x0000_0000_0000_0000 ~ (2^(64-T0SZ) - 1)
    // Upper VA subrange ： (2^64 - 2^(64-T1SZ)) ~  0xFFFF_FFFF_FFFF_FFFF
    r = //(0b00LL << 37) | // TBI=0, no tagging
        //(b << 32) |      // IPS=autodetected
        (0b10LL << 30) | // TG1=4k
        //(0b11LL << 28) | // SH1=3 inner
        //(0b01LL << 26) | // ORGN1=1 write back
        //(0b01LL << 24) | // IRGN1=1 write back
        //(0b0LL << 23) |  // EPD1 enable higher half
        // 25LL -> (64 - 48) = 16
        (16LL << 16) |   // T1SZ=25, 3 levels (512G)
        (0b00LL << 14) | // TG0=4k
        //(0b11LL << 12) | // SH0=3 inner
        //(0b01LL << 10) | // ORGN0=1 write back
        //(0b01LL << 8) |  // IRGN0=1 write back
        //(0b0LL << 7) |   // EPD0 enable lower half
        (16LL << 0); // T0SZ=25, 3 levels (512G)
    asm volatile("msr tcr_el1, %0; isb"
                 :
                 : "r"(r));

    // Require 1-2: Set up MAIR_EL1
#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1
    r = 0;
    r = (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) |
        (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8));
    asm volatile("msr mair_el1, %0"
                 :
                 : "r"(r));

    // Require 1-3: Set up identity mapping
    /* create MMU translation tables at _end */

    printf("%x\n", &_end);

    printf("%x\n", ((unsigned long)&_end + TTBR_CNP + PAGESIZE));
    printf("%x\n", ((unsigned long)&paging[512]));

#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)

    // TTBR0, PGD
    paging[0] = (unsigned long)((unsigned char *)&_end + 0x1000) | // physical address
                BOOT_PGD_ATTR;

    paging[0 + 512] = (unsigned long)((unsigned char *)0) | // physical address
                      BOOT_PUD_ATTR;

    paging[0 + 512 + 1] = (unsigned long)((unsigned char *)0x40000000) | // physical address
                          BOOT_PUD_ATTR;

    uart_send_hex(paging[0 + 512 + 0]);
    uart_send('\n');

    uart_send_hex(paging[0 + 512 + 1]);

    /*
    r = 0;
    *((unsigned long *)r) = 0x1000 | BOOT_PGD_ATTR;

    r = 0x1000;
    *((unsigned long *)r) = 0x00000000 | BOOT_PUD_ATTR;
    uart_send_hex(*((unsigned long *)r));

    r = 0x1000 + 8;
    *((unsigned long *)r) = 0x40000000 | BOOT_PUD_ATTR;
    */

    // Require 1-3: Set up identity mapping. Enable MMU
    // tell the MMU where our translation tables are. TTBR_CNP bit not documented, but required
    // lower half, user space
    asm volatile("msr ttbr0_el1, %0"
                 :
                 : "r"((unsigned long)&_end + TTBR_CNP));

    // upper half, kernel space
    asm volatile("msr ttbr1_el1, %0"
                 :
                 : "r"((unsigned long)&_end + TTBR_CNP));

    // finally, toggle some bits in system control register to enable page translation
    asm volatile("dsb ish; isb; mrs %0, sctlr_el1"
                 : "=r"(r));
    r |= (1 << 0); // set M, enable MMU

    asm volatile("msr sctlr_el1, %0; isb"
                 :
                 : "r"(r));
}
