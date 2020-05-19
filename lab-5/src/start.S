#include "register.h"
.section ".text.boot"

.global _start
_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f

    // cpu id > 0, stop
1:  wfe
    b       1b

2:  // b el2_setting
    // cpu id == 0
    // El2 to El1
    // set stack before our code
    ldr     x1, =_start
    msr     sp_el1, x1
    // set hcr_el2
    ldr     x0, =HCR_VALUE
    msr     hcr_el2, x0
    // set spsr_el2
    ldr	    x0, =SPSR_VALUE
	msr	    spsr_el2, x0
    // set up exception table
    ldr     x2, =exception_table_el1
    msr     vbar_el1, x2
    ldr     x2, =exception_table_el2
    msr     vbar_el2, x2
    // jump to el1_start when call eret
    // adr	    x0, el1_to_el0		
    adr	    x0, clear_bss		
	msr	    elr_el2, x0
    eret

el2_setting:
    // set HCR_EL2.IMO
    mrs     x0, hcr_el2
    orr     x0, x0, #(1 << 4)
    msr     hcr_el2, x0
    // clear PSTATE.DAIF
    mov     x0, #0
    msr     DAIF, x0
    // set up exception table
    ldr     x2, =exception_table_el1
    msr     vbar_el1, x2
    ldr     x2, =exception_table_el2
    msr     vbar_el2, x2
    // set stack
    ldr     x1, =_start
    mov     sp, x1
    b clear_bss
    
el1_to_el0: 
    // change exception level to EL0
    mov     x0, USER_STACK
    msr     sp_el0, x0
    mov     x0, 0 // EL0 with interrupt enabled
    msr     spsr_el1, x0
    adr	    x0, clear_bss
	msr	    elr_el1, x0
    eret
    
clear_bss:
    // bl      page_initialize 
    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, main_entry
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

main_entry:
    bl      main   // jump to C code, should not return
    b       1b     // for failsafe, halt this core too

page_initialize:
    // setup TCR
    ldr x0, = TCR_CONFIG_DEFAULT
    msr tcr_el1,x0

    // setup MAIR_EL1
    ldr x0, =( \
        (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
        (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0

    // setup paging
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
    
    msr ttbr0_el1, x0
    msr ttbr1_el1, x0 // also load PGD to the upper translation based register.
    mrs x2, sctlr_el1
    orr x2 , x2, 1
    msr sctlr_el1, x2
    ret