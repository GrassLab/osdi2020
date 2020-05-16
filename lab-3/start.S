#include "register.h"
.section ".text.boot"

.global _start
	.macro	kernel_entry
	sub	sp, sp, #256
	stp	x0, x1, [sp, #16 * 0]
	stp	x2, x3, [sp, #16 * 1]
	stp	x4, x5, [sp, #16 * 2]
	stp	x6, x7, [sp, #16 * 3]
	stp	x8, x9, [sp, #16 * 4]
	stp	x10, x11, [sp, #16 * 5]
	stp	x12, x13, [sp, #16 * 6]
	stp	x14, x15, [sp, #16 * 7]
	stp	x16, x17, [sp, #16 * 8]
	stp	x18, x19, [sp, #16 * 9]
	stp	x20, x21, [sp, #16 * 10]
	stp	x22, x23, [sp, #16 * 11]
	stp	x24, x25, [sp, #16 * 12]
	stp	x26, x27, [sp, #16 * 13]
	stp	x28, x29, [sp, #16 * 14]
	str	x30, [sp, #16 * 15] 
	.endm

    .macro	kernel_exit
	ldp	x0, x1, [sp, #16 * 0]
	ldp	x2, x3, [sp, #16 * 1]
	ldp	x4, x5, [sp, #16 * 2]
	ldp	x6, x7, [sp, #16 * 3]
	ldp	x8, x9, [sp, #16 * 4]
	ldp	x10, x11, [sp, #16 * 5]
	ldp	x12, x13, [sp, #16 * 6]
	ldp	x14, x15, [sp, #16 * 7]
	ldp	x16, x17, [sp, #16 * 8]
	ldp	x18, x19, [sp, #16 * 9]
	ldp	x20, x21, [sp, #16 * 10]
	ldp	x22, x23, [sp, #16 * 11]
	ldp	x24, x25, [sp, #16 * 12]
	ldp	x26, x27, [sp, #16 * 13]
	ldp	x28, x29, [sp, #16 * 14]
	ldr	x30, [sp, #16 * 15] 
	add	sp, sp, #256		
	eret
	.endm

	.macro	ventry	label
	.align	7
	b	\label
	.endm

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

    .align 11      // important, code has to be properly aligned
exception_table_el1:
    ventry el1_exception
    ventry el1_exception
    ventry el1_exception
    ventry el1_exception
    // synchronous
    ventry el1_exception
    // IRQ
    ventry el1_irq
    // FIQ
    ventry el1_exception
    // SError
    ventry el1_exception

    ventry el1_exception
    ventry el1_irq
    ventry el1_exception
    ventry el1_exception

    .align 11      // important, code has to be properly aligned
exception_table_el2:
    ventry el2_exception
    ventry el2_exception
    ventry el2_exception
    ventry el2_exception
    // synchronous
    ventry el2_exception
    // IRQ
    ventry el2_irq
    // FIQ
    ventry el2_exception
    // SError
    ventry el2_exception

    ventry el2_exception
    ventry el2_exception
    ventry el2_exception
    ventry el2_exception

el1_exception:
    kernel_entry
    bl exception_handler
    kernel_exit

el1_irq:
    kernel_entry
    bl irq_handler
    kernel_exit

el2_exception:
    kernel_entry
    bl exception_handler
    kernel_exit

el2_irq:
    kernel_entry
    bl irq_handler
    kernel_exit