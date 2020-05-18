.section ".text.boot.mmu"

#include "mmu.h"

.global mmu_init

.macro map_page_block, page_table_addr, phys_start, start, end, flags, tmp

    # get starting index
    lsr     \start, \start, SECTION_OFFSET
    and     \start, \start, TABLE_SIZE - 1      

    # get ending index
    lsr     \end,   \end,   SECTION_OFFSET
    and     \end,   \end,   TABLE_SIZE - 1     

    # prepare block descriptior
    lsr    \phys_start, \phys_start, SECTION_OFFSET
    mov    \tmp,    \flags

    # table entry
    orr    \phys_start, \tmp, \phys_start, lsl SECTION_OFFSET

9999:
    
    # store the entry
    str    \phys_start, [\page_table_addr, \start, lsl 3]                

    # next entry
    add    \start, \start, 1                    

    # next block
    add    \phys_start, \phys_start, SECTION_SIZE

    # cmpare and jump out
    cmp    \start, \end
    b.ls    9999b

.endm

mmu_init:

    mov     x29,    x30
    
    // clear table space as zero
    adrp    x0,     _PGD_START          // PGD's page frame, defined in linker script
    mov     x1,     PAGE_DIR_SIZE       
    bl      memzero
    
    # set up pgd table
    adrp    x0,     _PGD_START          // PGD's page frame, defined in linker script
    add     x1,     x0,     PAGE_SIZE   // PUD's page frame at PGD + 1 PAGE

    ldr     x2,     =( PGD_ATTR )       // get attribute of pgd
    orr     x2,     x1,     x2          // combine the address of next level page with attribute.
    str     x2,     [x0]                // store the pud address with attr in the first entry of pgd

    # set up pud table    
    add     x0,     x0,     PAGE_SIZE   // wher PUD start
    add     x1,     x0,     PAGE_SIZE   // PMD's page frame at PUD + 1 PAGE

    ldr     x2,     =( PUD_ATTR )       // get attribute of pud
    orr     x2,     x1,     x2          // combine the address of next level page with attribute.
    str     x2,     [x0]                // store the pmd address with attr in the first entry of pgd

    # set up pmd table
    # Mapping Kernel Memory    
    add     x0,     x0,     PAGE_SIZE       // wher PMD start
    mov     x1,     xzr                     // start mapping from 0
    mov     x2,     VA_START                // first virtual address
    ldr	    x3,     =( VA_START + DEVICE_BASE - SECTION_SIZE )  // last virtual address
    mov     x4,     PMD_ATTR_NORMAL         // descriptior
    map_page_block x0, x1, x2, x3, x4, x5

    # Mapping Device Memory
    adrp    x0,     _PGD_START          
    add     x0,     x0,     PAGE_SIZE * 2   // wher PMD start
    mov     x1,     DEVICE_BASE             // start mapping from device base
    ldr     x2,     =( VA_START + DEVICE_BASE )                         // first virtual address
    ldr	    x3,     =( VA_START + PHYS_MEMORY_SIZE - SECTION_SIZE )     // last virtual address
    mov     x4,     PMD_ATTR_DEVICE         // descriptior
    map_page_block x0, x1, x2, x3, x4, x5

    # set up TCR_EL1
    ldr     x0,     =( TCR_CONFIG_DEFAULT )
    msr     TCR_EL1,    x0
    
    # set up mair_el1
    ldr     x0,     =( MAIR_CONFIG_DEFAULT )
    msr     MAIR_EL1,   x0

    adrp    x0,     _PGD_START          
    msr     TTBR0_EL1, x0               // load PGD to the buttom translation based register.
    msr     TTBR1_EL1, x0               // load PGD to the buttom translation based register.

    mov     x30,    x29

    ret