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
	b	   \label
	.endm

_start:
    mrs    x0, mpidr_el1
    and    x0, x0, #3 
    cbz    x0, set_stack_bss //if x0 == 0 jump to forward of set_stack_bss
    b      loop

loop:  
    wfe
    b      loop //busy loop

set_stack_bss:
    //if cpuid == 0, set stack 
    ldr    x0, =_start
    mov    sp, x0
    //clear bss
    ldr    x0, =__bss_start
    ldr    w1, =__bss_size

clear_bss:  
    //if w1 == 0,
    cbz    w1, j_main
    // move xzr to x0 addr and add x0 by 8
    // set x0 to xzr(zero) and add by 9
    str    xzr, [x0], #8
    //sub bss_size until bss_size = 0
    sub    w1, w1, #1
    cbnz   w1, clear_bss

check_EL2:
    mrs    x0, CurrentEL
    lsr    x0, x0, #2
    cmp    x0, #2
    bne    setup_msg_table

setup_exception_vector_table:
    ldr    x0, =exception_table
    msr    VBAR_EL2, x0
    mov    x0, xzr

j_main:  
    //jump to c code
    bl     main
    b      loop

setup_msg_table:
    mov    x0, #1
    b      j_main

.align 11
exception_table:
    ventry not_implemented
    ventry not_implemented
    ventry not_implemented
    ventry not_implemented

    ventry el1_exception
    ventry not_implemented
    ventry not_implemented
    ventry not_implemented

    ventry not_implemented
    ventry not_implemented
    ventry not_implemented
    ventry not_implemented

    ventry not_implemented
    ventry not_implemented
    ventry not_implemented
    ventry not_implemented

el1_exception:
    kernel_entry
    mrs x0, elr_el2
    mrs x1, esr_el2
    bl exception_handler
    kernel_exit

.global branch_to_address
branch_to_address:
    br x0
    ret // We never reach this point
