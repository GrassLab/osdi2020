.section ".text.boot.mmu"

#include "mmu.h"

.global mmu_init

mmu_init:

    // save return address
    // there is no function call inside, not necessary to do this
    // mov     x29,    x30
  
    # set up TCR_EL1
    ldr     x0,     =( TCR_CONFIG_DEFAULT )
    msr     TCR_EL1,    x0
    
    # set up mair_el1
    ldr     x0,     =( MAIR_CONFIG_DEFAULT )
    msr     MAIR_EL1,   x0

    # set up identity mapping
    # The first entry of PGD which points to PUD
    # The first entry of PUD maps 0x00000000 - 0x3fffffff (RAM and GPU peripherals)
    # The second entry of PUD maps 0x40000000 - 0x7fffffff(ARM local peripherals).
    # set up PGD PUD
    adrp    x0,     _PGD_START          // PGD's page frame, defined in linker script
    add     x1,     x0,     PAGE_SIZE   // PUD's page frame at PGD + 1PAGE
    
    ldr     x2,     =( PGD_ATTR )
    orr     x2,     x1,     x2          // combine the physical address of next level page with attribute.
    str     x2,     [x0]                // store the pud address with attr in the first entry of pgd
    
    ldr     x2,     =( PUD_ATTR )
    mov     x3,     0x00000000          // mapped 0x00000000 - 0x3fffffff
    orr     x3,     x2,     x3          // combine the physical address with attribute
    str     x3,     [x1]                // 1st 1GB mapped by the 1st entry of PUD
    mov     x3,     0x40000000          // mapped 0x40000000 - 0x7fffffff
    orr     x3,     x2,     x3          // combine the phusical address with attribute
    str     x3,     [x1, 8]             // 2nd 1GB mapped by the 2nd entry of PUD
    
    msr     TTBR0_EL1, x0               // load PGD to the buttom translation based register.
    msr     TTBR1_EL1, x0               // load PGD to the buttom translation based register.
    
    ret