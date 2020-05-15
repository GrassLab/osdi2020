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
    /*
    // TTBR0, PGD
    paging[0] = (unsigned long)((unsigned char *)&_end + 0x1000) | // physical address
                PT_PGD |                                           // it has the "Present" flag, which must be set, and we have area in it mapped by pages
                PT_AF |                                            // accessed flag. Without this we're going to have a Data Abort exception
                PT_USER |                                          // non-privileged
                PT_ISH |                                           // inner shareable
                PT_MEM;                                            // normal memory

    //
    // 1st PUD, 1GB address
    // bottom, 0x0000000000000000 ~ 0xffff000000000000
    paging[PAGE] = (unsigned long)((unsigned char *)&paging[2 * PAGE]) | // physical address
                   PT_PUD |                                              // it has the "Present" flag, which must be set, and we have area in it mapped by pages
                   PT_AF |                                               // accessed flag. Without this we're going to have a Data Abort exception
                   PT_USER |                                             // non-privileged
                   PT_ISH |                                              // inner shareable
                   PT_MEM;                                               // normal memory

    // 2nd PUD, 1GB address
    // upper, 0xffff000000000000 ~ 0xffffffffffffffff
    paging[PAGE + 1] = (unsigned long)((unsigned char *)&paging[2 * PAGE]) | // physical address
                       PT_PUD |                                              // it has the "Present" flag, which must be set, and we have area in it mapped by pages
                       PT_AF |                                               // accessed flag. Without this we're going to have a Data Abort exception
                       PT_USER |                                             // non-privileged
                       PT_ISH |                                              // inner shareable
                       PT_MEM;                                               // normal memory

    // PMD
    // 2 * PAGE ~ (2 + 511) * PAGE
    unsigned long *pmd = &paging[2 * PAGE];
    for (int i = 0; i < 512; i++)
    {
        *(pmd + i) = (unsigned long)((unsigned char *)(pmd + PAGE)) | // physical address
                     PT_PMD |                                         // map 2M block
                     PT_AF |                                          // accessed flag
                     PT_NX |                                          // no execute
                     PT_USER |                                        // non-privileged
                     (r >= b ? PT_OSH | PT_DEV : PT_ISH | PT_MEM);    // different attributes for device memory
    }

    // PTE
    b = MMIO_BASE >> 21;
    unsigned long *ptd = &paging[3 * PAGE];
    for (int i = 0; i < 512; i++)
    {
        for (int j = 0; j < 512; j++)
        {
            *(ptd + i * PAGE + j) = (unsigned long)(unsigned char *)(i * PAGESIZE * PAGESIZE + j * PAGESIZE) | // physical address
                                    PT_PTE |                                                                   // map 2M block
                                    PT_AF |                                                                    // accessed flag
                                    PT_NX |                                                                    // no execute
                                    PT_USER |                                                                  // non-privileged
                                    (r >= b ? PT_OSH | PT_DEV : PT_ISH | PT_MEM);                              // different attributes for device memory
        }
    }
    */

    // TTBR0, identity L1
    paging[0] = (unsigned long)((unsigned char *)&paging[0] + 2 * PAGESIZE) | // physical address
                PT_PGD |                                                      // it has the "Present" flag, which must be set, and we have area in it mapped by pages
                PT_AF |                                                       // accessed flag. Without this we're going to have a Data Abort exception
                PT_USER |                                                     // non-privileged
                PT_ISH |                                                      // inner shareable
                PT_MEM;                                                       // normal memory

    // identity L2, first 2M block
    paging[2 * 512] = (unsigned long)((unsigned char *)&paging[0] + 3 * PAGESIZE) | // physical address
                      PT_PGD |                                                      // we have area in it mapped by pages
                      PT_AF |                                                       // accessed flag
                      PT_USER |                                                     // non-privileged
                      PT_ISH |                                                      // inner shareable
                      PT_MEM;                                                       // normal memory

    printf("%x\n", &_end + 2 * PAGESIZE);
    printf("%x\n", &paging[2 * 512]);

    // identity L2 2M blocks
    b = MMIO_BASE >> 21;
    // skip 0th, as we're about to map it by L3
    // Map 1GB
    // VA -> PA
    // 0x0000000000000000 -> 0x0000000000000000
    for (r = 1; r < 512; r++)
    {
        //uart_send_hex(r << 21);
        //uart_send('\n');
        paging[2 * 512 + r] = (unsigned long)((r << 21)) | // physical address
                              PT_PUD |                     // map 2M block
                              PT_AF |                      // accessed flag
                              PT_NX |                      // no execute
                              PT_USER |                    // non-privileged
                              //(r >= b ? PT_OSH | PT_DEV : PT_ISH | PT_MEM); // different attributes for device memory
                              PT_ISH | PT_MEM;
    }

    // identity L3
    for (r = 0; r < 512; r++)
        paging[3 * 512 + r] = (unsigned long)(r * PAGESIZE) |                         // physical address
                              PT_PTE |                                                // map 4k
                              PT_AF |                                                 // accessed flag
                              PT_USER |                                               // non-privileged
                              PT_ISH |                                                // inner shareable
                              ((r < 0x80 || r >= data_page) ? PT_RW | PT_NX : PT_RO); // different for code and data
    // TTBR1, kernel L1
    // block, 1GB
    paging[512 + 0] = (unsigned long)((unsigned char *)&paging[0] + 4 * PAGESIZE) | // physical address
                      PT_PGD |                                                      // we have area in it mapped by pages
                      PT_AF |                                                       // accessed flag
                      PT_USER |
                      PT_ISH | // inner shareable
                      PT_MEM;  // normal memory

    // kernel L2
    // block, 2MB
    paging[4 * 512 + 0] = (unsigned long)((unsigned char *)&paging[0] + 5 * PAGESIZE) | // physical address
                          PT_PUD |                                                      // we have area in it mapped by pages
                          PT_AF |                                                       // accessed flag
                          PT_USER |
                          PT_ISH | // inner shareable
                          PT_MEM;  // normal memory

    // kernel L3
    //paging[6 * 512 + 0] = (unsigned long)(MMIO_BASE + 0x00201000) | // physical address
    paging[5 * 512 + 0] = (unsigned long)(&paging[0] + 10 * PAGESIZE) | // physical address
                          PT_PTE |                                      // map 4k
                          PT_AF |                                       // accessed flag
                          PT_NX |                                       // no execute
                          PT_USER |
                          PT_OSH | // outter shareable
                          PT_DEV;  // device memory
    /*
    T1SZ = 25
    
    #define UART0_DR ((volatile unsigned int *)(MMIO_BASE + 0x00201000))
    #define UART0_FR ((volatile unsigned int *)(MMIO_BASE + 0x00201018))

    KERNEL_UART0_DR = ((volatile unsigned int *)0xFFFFFFFFFFE00000)
    KERNEL_UART0_FR = ((volatile unsigned int *)0xFFFFFFFFFFE00018)
    // TTBR1, kernel L1
    paging[512 + 511] = (unsigned long)((unsigned char *)&_end + 4 * PAGESIZE) | // physical address
                        PT_PAGE |                                                // we have area in it mapped by pages
                        PT_AF |                                                  // accessed flag
                        PT_KERNEL |                                              // privileged
                        PT_ISH |                                                 // inner shareable
                        PT_MEM;                                                  // normal memory

    // kernel L2
    paging[4 * 512 + 511] = (unsigned long)((unsigned char *)&_end + 5 * PAGESIZE) | // physical address
                            PT_PAGE |                                                // we have area in it mapped by pages
                            PT_AF |                                                  // accessed flag
                            PT_KERNEL |                                              // privileged
                            PT_ISH |                                                 // inner shareable
                            PT_MEM;                                                  // normal memory

    // kernel L3
    paging[5 * 512] = (unsigned long)(MMIO_BASE + 0x00201000) | // physical address
                      PT_PAGE |                                 // map 4k
                      PT_AF |                                   // accessed flag
                      PT_NX |                                   // no execute
                      PT_KERNEL |                               // privileged
                      PT_OSH |                                  // outter shareable
                      PT_DEV;                                   // device memory
                          */

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
                 : "r"((unsigned long)&_end + TTBR_CNP + PAGESIZE));

    // finally, toggle some bits in system control register to enable page translation
    asm volatile("dsb ish; isb; mrs %0, sctlr_el1"
                 : "=r"(r));
    r |= (1 << 0); // set M, enable MMU

    asm volatile("msr sctlr_el1, %0; isb"
                 :
                 : "r"(r));
}
