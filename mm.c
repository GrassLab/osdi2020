#include "gpio.h" // get MMIO_BASE
#include "uart.h"

#include "printf.h"
#include "mm.h"

// get addresses from linker
extern volatile unsigned char _data;
extern volatile unsigned char _end;

typedef struct page_t
{
    unsigned long pfn;
    unsigned long pa;
    int used;
} page_t;

page_t page_table[1000];

unsigned long pa_to_pfn(unsigned long pa)
{
    /*
    _uart_send('$');
    uart_send_hex((unsigned long)(mem_map_pa));
    _uart_send('$');
    uart_send_hex((unsigned long)(pa - mem_map_pa));
    _uart_send('$');
    */
    return (pa - mem_map_pa) / PAGE_SIZE;
    // return ((pa - 0xffff000000000000) - (unsigned long)&mem_map[0]) / PAGE_SIZE;
}

unsigned long va_to_pa(unsigned long va)
{
    /*
    _uart_send('!');
    uart_send_int((va - 0xffff000000000000) / PAGE_SIZE);
    _uart_send('!');
    uart_send_hex(page_table[(va - 0xffff000000000000) / PAGE_SIZE].pa);
    _uart_send('!');
    */

    return page_table[(va - 0xffff000000000000) / PAGE_SIZE].pa;
}

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
#define BOOT_PGD_ATTR PT_TABLE
#define BOOT_PUD_ATTR (PT_AF | (MAIR_IDX_DEVICE_nGnRnE << 2) | PT_BLOCK)
#define BOOT_PMD_ATTR (PT_AF | (MAIR_IDX_DEVICE_nGnRnE << 2) | PT_BLOCK)
#define BOOT_PTE_ATTR (PT_AF | PT_TABLE)

    // TTBR0, PGD
    paging[0] = (unsigned long)(&paging[0 + 512]) | // to PUD table
                PT_TABLE | PT_AF;

    // PUD: 1GB
    paging[512] = (unsigned long)(&paging[512 * 2]) | // to PMD table
                  PT_TABLE | PT_AF;
    /*
    paging[0 + 512] = (unsigned long)((unsigned char *)0x0) | // physical address
                      BOOT_PUD_ATTR;
                      */
    /*
    paging[0 + 512] = (unsigned long)((unsigned char *)&paging[0 + 512 * 2]) |
                      BOOT_PUD_ATTR;
                      */

    // PUD: 1GB
    paging[512 + 1] = (unsigned long)(0x40000000) | // physical address
                      BOOT_PUD_ATTR;

    // PMD: 2MB
    // table -> PTE
    for (int i = 0; i < 512; i++)
    {
        paging[512 * 2 + i] = (unsigned long)(&paging[512 * (3 + i)]) |
                              PT_TABLE | PT_AF;
    }

    /*
    // PMD: 2MB
    // block -> physical address
    for (int i = 1; i < 512; i++)
    {
        paging[512 * 2 + i] = (unsigned long)((1 << 21) * i) | // physical address
                              PT_BLOCK | PT_AF;
    }
    */

    // PTE: 4KB
    for (int n = 0; n < 512; n++)
    {
        for (int i = 0; i < 512; i++)
        {
            paging[512 * (3 + n) + i] = (unsigned long)((1 << 21) * n + i * PAGE_SIZE) | // physical address;
                                        PT_PAGE | PT_AF;
        }
    }

    /*
    // PMD 1:
    paging[0 + 512 * 2] = (unsigned long)((unsigned char *)&paging[0 + 512 * 4]) | // physical address
                          BOOT_PMD_ATTR;

    paging[0 + 512 * 3] = (unsigned long)((unsigned char *)&paging[0 + 512 * 5]) | // physical address
                          BOOT_PMD_ATTR;

    for (int i = 0; i < 512; i++)
    {
        paging[0 + 512 * 4 + i] = (unsigned long)((unsigned char *)((1 << 21) * i)) | // physical address
                                  BOOT_PTE_ATTR;
    }
    for (int i = 0; i < 512; i++)
    {
        paging[0 + 512 * 5 + i] = (unsigned long)((unsigned char *)(0x40000000 + (1 << 21) * i)) | // physical address
                                  BOOT_PTE_ATTR;
    }
    */

    // Memory Page
    // TTBR1, PGD
    /*
    paging[512 * 5] = (unsigned long)((unsigned char *)&paging[512 * 6]) | // to PUD
                      PT_TABLE | PT_AF;
    // PUD: 1GB
    paging[512 * 6] = (unsigned long)((unsigned char *)&paging[512 * 7]) | // to PMD
                      PT_TABLE | PT_AF;

    // PMD: 2MB
    paging[512 * 7] = (unsigned long)((unsigned char *)&paging[512 * 9]) | // to PTE
                      PT_TABLE | PT_AF;
    paging[512 * 8] = (unsigned long)((unsigned char *)&paging[512 * 10]) | // to PTE
                      PT_TABLE | PT_AF;

    // PTE: 4KB
    for (int i = 0; i < 512; i++)
    {
        paging[512 * 9 + i] = (unsigned long)((unsigned char *)mem_map_pa + i * PAGE_SIZE) | // physical address
                              PT_PAGE | PT_AF;
    }
    // PTE: 4KB
    for (int i = 0; i < 512; i++)
    {
        paging[512 * 10 + i] = (unsigned long)((unsigned char *)mem_map_pa + (512 + i) * PAGE_SIZE) | // physical address
                               PT_PAGE | PT_AF;
    }
    */

    mem_map_init();


    // Require 1-3: Set up identity mapping. Enable MMU
    // tell the MMU where our translation tables are. TTBR_CNP bit not documented, but required
    // lower half, kernel space
    asm volatile("msr ttbr0_el1, %0"
                 :
                 : "r"((unsigned long)(&paging[0] + TTBR_CNP)));

    // upper half, user space
    /*
    asm volatile("msr ttbr1_el1, %0"
                 :
                 : "r"((unsigned long)(&paging[512 * 5] + TTBR_CNP)));
                 */

    // finally, toggle some bits in system control register to enable page translation
    asm volatile("dsb ish; isb; mrs %0, sctlr_el1"
                 : "=r"(r));
    r |= (1 << 0); // set M, enable MMU

    asm volatile("msr sctlr_el1, %0; isb"
                 :
                 : "r"(r));
}

void mem_map_init()
{
    for (int i = 0; i < 1000; i++)
    {
        //page_table[i].pa = (unsigned long)&mem_map[i];
        page_table[i].pa = (unsigned long)((unsigned char *)mem_map_pa + i * PAGE_SIZE);
        page_table[i].pfn = i;
        page_table[i].used = 0;
        *(unsigned long *)(page_table[i].pa) = 10000 + i; // for test memory map
    }
}

unsigned long get_free_page()
{
    for (int i = 0; i < PAGING_PAGES; i++)
    {
        if (page_table[i].used == 0)
        {
            page_table[i].used = 1;
            //page_table[i].pa = (unsigned long)&mem_map[i] + i * PAGE_SIZE;

            // return LOW_MEMORY + i * PAGE_SIZE;

            /*
            uart_send('(');
            uart_send_hex((unsigned long)((unsigned char *)mem_map_pa + i * PAGE_SIZE) >> 32);
            uart_send_hex(((unsigned char *)mem_map_pa + i * PAGE_SIZE) );
            uart_send(')');
            */

            return (unsigned long)((unsigned char *)mem_map_pa + i * PAGE_SIZE);

            // return 0xffff000000000000 + i * PAGE_SIZE;
            // return 0xffff000000000000 + i * PAGE_SIZE + PAGE_SIZE;
            // return 0xffff000000000000 + page_table[i].pa;
        }
    }
    return 0;
}

void free_page(unsigned long pfn)
{
    page_table[pfn].used = 0;

    uart_puts("free page: ");
    uart_send_int(pfn);
    _uart_send('\n');

    /*
    for (int i = 0; i < PAGING_PAGES; i++)
    {
        if (page_table[i].pa == (p - 0xffff000000000000))
        {
            page_table[i].used = 0;
        }
    }
    */
}

int mm_remain_page_num(){
    int num = 0;
    for(int i = 0; i < 1000; i++){
        if(page_table[i].used == 0){
            num++;
        }
    }
    return num;
}