#include "sysregs.h"
#include "mm.h"
#include "mmu.h"


.section ".text.boot"

.global _start
_start:
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF         // get CPU ID and mask reserved bits
    cbz     x0, master

pe_hang:                          // hang for all non-primary CPU
    wfe                           // enter low power mode
    b       pe_hang

master:
    ldr     x0, =SCTLR_VALUE_MMU_DISABLED
    msr     sctlr_el1, x0

    ldr     x0, =HCR_EL2_VALUE    // set EL1 aarch
    msr     hcr_el2, x0

    ldr     x0, =SPSR_EL2_VALUE   // set processor restore state after calling eret  
    msr     spsr_el2, x0

    adr     x0, el1_entry         // eret return address
    msr     elr_el2, x0

    ldr     x0, =LOW_MEMORY
    msr     sp_el1, x0

    eret                          // from el2 to el1

el1_entry:
    ldr     x0, =TCR_VALUE
    msr     tcr_el1, x0   

    ldr     x0, =MAIR_VALUE
    msr     mair_el1, x0
    // invalidate non used PGD PUD entry
    mov     x0, PGD_BASE    
    mov     x1, (2*PAGE_TABLE_SIZE)
    bl      memzero_64

    mov     x1, PUD_BASE
    mov     x2, PMD_BASE

    // setup first PGD entry(pointer to PUD)    
    ldr     x7, =PT_DESC_TO_TABLE
    orr     x7, x7, x1
    str     x7, [x0]
    // setup first PUD entry(pointer to PMD)
    ldr     x7, =PT_DESC_TO_TABLE
    orr     x7, x7, x2
    str     x7, [x1]
    // setup second PUD entry(pointer to local device)
    ldr     x7, =(PT_DEV_UPPER_ATTR | PT_DEV_LOWER_ATTR | PT_DESC_TO_BLOCK)
    orr     x7, x7, #LOCAL_DEV_BASE
    str     x7, [x1, #8]

    ldr     x7, =(PT_NOR_UPPER_ATTR | PT_NOR_LOWER_ATTR | PT_DESC_TO_BLOCK)
    mov     x6, xzr               // PMD idx conter
nor_pmd:                          // 0x000 00000 to 0x3ef fffff
    mov     x5, x6, lsl #BLOCK_SHIFT// block basis
    orr     x5, x5, x7
    str     x5, [x2, x6, lsl #3]
    add     x6, x6, #1
    cmp     x6, #(HIGH_MEMORY>>BLOCK_SHIFT)
    blo     nor_pmd

    ldr     x7, =(PT_DEV_UPPER_ATTR | PT_DEV_LOWER_ATTR | PT_DESC_TO_BLOCK)
dev_pmd:                          // 0x3F0 00000 to 0x3FF FFFFF
    mov     x5, x6, lsl #BLOCK_SHIFT// block basis
    orr     x5, x5, x7            // combine block address and attribute to a descriptor
    str     x5, [x2, x6, lsl #3]
    add     x6, x6, #1
    cmp     x6, #(LOCAL_DEV_BASE>>BLOCK_SHIFT)
    blo     dev_pmd


    msr     ttbr0_el1, x0         // load PGD to the buttom translation based register.
    msr     ttbr1_el1, x0         // also load PGD to the upper translation based register.

    isb
    mrs     x0, sctlr_el1
    orr     x0 , x0, SCTLR_MMU_ENABLED
    msr     sctlr_el1, x0         // enable MMU, cache remains disabled
    isb

    // jump to 
    ldr     x2, =main_entry         // indirect branch to the virtual address(Note: bl is pc relative)
    br      x2


main_entry:
    mov     x0, #KVA_BASE
    add     sp, sp, x0
    // clear_bss
    ldr     x1, =__bss_size
    cbz     x1, main_entry
    ldr     x0, =__bss_start
    bl      memzero_64

    bl      kernel_main           // jump to C code, should not return
    b       pe_hang               // for failsafe, halt this core too