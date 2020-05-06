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
    adr	    x0, main_start		
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
    b main_start
    
el1_to_el0: 
    // change exception level to EL0
    mov     x0, USER_STACK
    msr     sp_el0, x0
    mov     x0, 0 // EL0 with interrupt enabled
    msr     spsr_el1, x0
    adr	    x0, main_start
	msr	    elr_el1, x0
    eret
    
main_start:
    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

4:  bl      main   // jump to C code, should not return
    b       1b     // for failsafe, halt this core too