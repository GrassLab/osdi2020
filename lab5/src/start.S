
.section ".text.boot"

#include "mmu.h"

.global _start
    
_start:
    mrs    x0, mpidr_el1
    and    x0, x0, #3 
    cbz    x0, master //if x0 == 0 jump to forward of set_stack_bss
    b      loop

loop:  
    wfe
    b      loop //busy loop

master:
    //Translation Control Register
    ldr x0, = TCR_CONFIG_DEFAULT
    msr tcr_el1, x0

    ldr x0, =( \
        (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
        (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0

    mov x0, 0 // PGD's page frame at 0x0
    mov x1, 0x1000 // PUD's page frame at 0x1000

    ldr x2, = BOOT_PGD_ATTR
    orr x2, x1, x2 // combine the physical address of next level page with attribute.
    str x2, [x0]

    ldr x2, = BOOT_PUD_ATTR
    mov x3, 0x00000000
    orr x3, x2, x3
    str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
    mov x3, 0x40000000
    orr x3, x2, x3
    str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

    msr ttbr0_el1, x0 // load PGD to the buttom translation based register.

    mrs x2, sctlr_el1
    orr x2 , x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled

    // setup exception vector for EL1
    ldr x0, =exception_table
    msr VBAR_EL1, x0

    // set rw bit in hcr_el2 because we're using 64-bit kernel    
    mov    x0, #(1 << 31)
    msr    hcr_el2, x0

    // set stack pointer to __bss_end__ + 0x4000
    ldr x0, =_start
    add x0, x0, #0x4000
    mov sp, x0

    // setup sp_el for el0 and el1, el2 need to be set in el3
    add x0, x0, #0x2000
    msr sp_el0, x0
    add x0, x0, #0x2000
    msr sp_el1, x0

    // initialize .bss section
    ldr    x0, =__bss_start
    ldr    w1, =__bss_size

clear_bss:  
    //if w1 == 0,
    cbz    w1, el2_to_el1
    // move xzr to x0 addr and add x0 by 8
    // set x0 to xzr(zero) and add by 9
    str    xzr, [x0], #8
    //sub bss_size until bss_size = 0
    sub    w1, w1, #1
    cbnz   w1, clear_bss
/* 
el2_to_el1:
    // Use el1h -> sp_el1 -> 0x5 M[3:0], bits [3:0] 0b0101 EL1h. h表示SP_ELx
    mov x0, 0x5
    msr spsr_el2, x0
    // Use eret to el1
    adr x0, main
    msr elr_el2, x0
    eret
*/
/* */
el2_to_el1:
    // Use el1h -> sp_el1 -> 0x5 M[3:0], bits [3:0] 0b0101 EL1h. h表示SP_ELx
    mov x0, 0x5
    msr spsr_el2, x0
    // Use eret to el1
    adr x0, el1_to_el0
    msr elr_el2, x0
    eret

el1_to_el0:
    eor x0, x0, x0
    msr spsr_el1, x0
    adr x0, main
    msr elr_el1, x0
    eret 
