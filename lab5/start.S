#include "sysregs.h"
#include "mmu_size.h"

.section ".text.boot"

.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, boot
    // cpu id > 0, stop
busy_loop:  
    wfe
    b       busy_loop
    
boot:
    //disable MMU
    ldr     x0, = SCTLR_MMU_DISABLE
    msr     sctlr_el1, x0

    /*
        exception level switch to El1
    */
    ldr     x0, = HCR_VALUE           
    msr     hcr_el2, x0

    ldr     x0, = SPSR_EL2_VAL              
    msr     spsr_el2, x0

    adr     x0, EL2_to_El1          // load exception return address
    msr     elr_el2, x0

    adr     x0, _start              // init sp for el1 option 1
    msr     sp_el1, x0

    ldr     x2, =__vectors1
    msr     vbar_el2, x2

    eret                           // return to EL1

EL2_to_El1:
    ldr     x0, = SPSR_EL1_VAL
    msr     spsr_el1, x0
    bl      enable_interrupt
    //bl      enable_EL0_interrupt

    /*
        clear bss
    */
    adrp     x1, __bss_start
    adrp     x2, __bss_end
    sub     x2, x2, x1
    lsr     x2, x2,3
    cbz     x2, clear_bss_end
clear_bss:  
    str     xzr, [x1], #8
    sub     x2, x2, #1
    cbnz    x2, clear_bss

clear_bss_end:
    bl      mmu_init

    ldr     x0, =_start
    mov     sp, x0

    ldr     x1, = kernel_main

    ldr     x0, = SCTLR_MMU_ENABLE
    msr     sctlr_el1, x0 
    
    br      x1
    

