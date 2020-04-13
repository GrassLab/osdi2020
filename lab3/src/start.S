.section ".text.boot"

.global _start

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
    msr    vbar_EL2, x0
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
    b      not_implemented
    .align 7
    b      not_implemented
    .align 7
    b      not_implemented
    .align 7
    b      not_implemented
    .align 7

    b      exception_handler
    .align 7
    b      not_implemented
    .align 7
    b      not_implemented
    .align 7
    b      not_implemented
    .align 7

    b      not_implemented
    .align 7
    b      not_implemented
    .align 7
    b      not_implemented
    .align 7
    b      not_implemented
    .align 7

    b      not_implemented
    .align 7
    b      not_implemented
    .align 7
    b      not_implemented
    .align 7
    b      not_implemented
    .align 7


.global branch_to_address
branch_to_address:
    br x0
    ret // We never reach this point
