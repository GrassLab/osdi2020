.section ".text.boot"


#define S_FRAME_SIZE 256

.macro context_switch
sub	sp, sp, #S_FRAME_SIZE
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

.macro	context_switch_back
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
add	sp, sp, #S_FRAME_SIZE		
eret
.endm

.global _start

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

5:  // set HCR_EL2.IMO
    mrs     x0, hcr_el2
    orr     x0, x0, #(1 << 4)
    orr     x0, x0, #0x8000000
    msr     hcr_el2, x0

    // for debug not route to El1
    // mrs x0, MDCR_EL2
    // bic x0, x0, 0x100
    // msr MDCR_EL2, x0

    // clear PSTATE.DAIF
    mov     x0, #0
    msr     DAIF, x0

    // set up exception handlers
    ldr     x2, =exception_table
    msr     vbar_el2, x2

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

.globl irq_vector_init
irq_vector_init:
    mrs     x0, hcr_el2
    orr     x0, x0, #(1 << 4)
    orr     x0, x0, #0x8000000
    msr     hcr_el2, x0

    mov     x0, #0
    msr     DAIF, x0

	adr	x2, exception_table		// load VBAR_EL1 with virtual
	msr	vbar_el2, x2		// vector table address
	// ret


// Simple vector table
.align 11 // vector table should be aligned to 0x800
exception_table:
    b exception_handler // branch to a handler function.
    .align 7 // entry size is 0x80, .align will pad 0
    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7

    b exception_handler
    .align 7

    context_switch
    bl exception_handler
    context_switch_back
    .align 7

    b exception_handler
    .align 7
    b exception_handler
    .align 7

    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7

    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7


el1_irq:
    context_switch
    bl handle_irq
    context_switch_back

exception_handler:
    context_switch
    mov     x0, #0
    mrs     x1, esr_el2
    mrs     x2, elr_el2
    mrs     x3, spsr_el2
    mrs     x4, far_el2
    bl _exception_handler
    context_switch_back

sync_currentEL_SP_ELx:
    context_switch
    mrs x0, ELR_EL2
    mrs x1, ESR_EL2
    bl el2_current_sp2_sync_message
    context_switch_back

proc_hang:
    b proc_hang

