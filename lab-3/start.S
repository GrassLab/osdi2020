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

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    // cpu id > 0, stop
1:  wfe
    b       1b
2:  // cpu id == 0
    // set stack before our code
    ldr     x1, =_start

    // set up EL1
    mrs     x0, CurrentEL
    and     x0, x0, #12 // clear reserved bits

    // running at EL2?
5:  cmp     x0, #4
    beq     5f
    msr     sp_el1, x1

    // set hcr_el2
    ldr     x0, =HCR_VALUE
    msr     hcr_el2, x0
    
    // set spsr_el2
    ldr	    x0, =SPSR_VALUE
	msr	    spsr_el2, x0
    // clear 
    mov     x0, #0
    msr     DAIF, x0
    // set up exception handlers
    ldr     x2, =exception_table
    msr     vbar_el1, x2
    eret

//5:  // set HCR_EL2.IMO
    //mrs     x0, hcr_el2
    //orr     x0, x0, #(1 << 4)
    //orr     x0, x0, #0x8000000
    //msr     hcr_el2, x0

    // clear PSTATE.DAIF
    //mov     x0, #0
    //msr     DAIF, x0

    // set up exception handlers
    //ldr     x2, =exception_table
    //msr     vbar_el2, x2

5:  mov     sp, x1 
    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

    // jump to C code, should not return
4:  bl      main
    // for failsafe, halt this core too
    b       1b

    // important, code has to be properly aligned
    .align 11
exception_table:
    b exception_handler
    .align 7
    bl exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler

    // synchronous
    .align  7
    kernel_entry
    mov     x0, #0
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    mrs     x3, spsr_el1
    mrs     x4, far_el1
    bl       exception_handler
    kernel_exit

    // IRQ
    .align  7
    kernel_entry
    bl      irq_handler//  core_timer_handler
    kernel_exit

    // FIQ
    .align  7
    mov     x0, #2
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    mrs     x3, spsr_el1
    mrs     x4, far_el1
    b       exception_handler

    // SError
    .align  7
    mov     x0, #3
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    mrs     x3, spsr_el1
    mrs     x4, far_el1
    b       exception_handler