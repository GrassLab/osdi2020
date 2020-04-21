.section ".text"            // start of the text segment
_start:                     // entry point for the linker
    mrs    x1, mpidr_el1    // load the multiprocessor affinity register
    and    x1, x1, #3       // OxFF mask to get the cpu id
    cbz    x1, setup_el2    // if cpuid is 0, start the booting process

/* 
 * Enter into low power standby mode.
 * It makes the cpu enter the STANDBYWFE state, which suspends execution,
 * unless an event is signaled by the SEV instruction or an interrupt happends.
 * This is a better alternative than using a simple NOP operation
 * because the latter would use all the cpu ressources for an infinite loop
 *
 * source:  http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka15473.html
 */
busy_loop:
    wfe
    b       busy_loop	    // branch back into busy_loop to make the cpu idle forever

setup_bss:
    ldr     x1, = __bss_start
    ldr     w2, = __bss_size

mem_zero:
    cbz     w2, setup_el2 
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, mem_zero

setup_el2:

    // setup the exception table in el1
    ldr     x1, = exception_table
    msr     VBAR_EL1, x1

    msr     SCTLR_EL1, xzr

    // enable HCR_EL2.RW (bit 31) for aarch64
    mrs     x0, HCR_EL2
    orr     x0, x0, #(1 << 31)
    msr     HCR_EL2, x0
    
    // reset DAIF
    mov     x0, #0b00101
    msr     SPSR_EL2, x0

    // return address
    adr     x0, setup_el1 
    msr     ELR_EL2, x0

    // el1 stack pointer
    ldr x1, =_start
    msr SP_EL1, x1
    
    // return to el1
    eret
 
setup_el1:

    mov     x0, 0x10000
    msr     SP_EL0, x0

    // Disable SMID
    mrs x0, CPACR_EL1
    orr x0, x0, #(3 << 20)
    msr CPACR_EL1, x0

    // enable EL0
    mov     x0, #0b00000
    msr     SPSR_EL1, x0

    // return address
    adr     x0, start_main
    msr     ELR_EL1, x0
    
    // return to el0
    eret

start_main:
    bl      ker_main        // branch and link to the C code
    b       busy_loop       // if something goes wrong, get in the busy loop

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

irq_entry:
    kernel_entry
    bl      irq_handler
    kernel_exit

exception_entry:
    kernel_entry
    bl      exception_handler
    kernel_exit

.global init_irq
init_irq:
    mrs x0, hcr_el2
    orr x0, x0, #16
    msr hcr_el2, x0
    ret

.global enable_irq
enable_irq:
	msr     daifclr, #0xF
    ret

.global disable_irq
disable_irq:
	msr     daifset, #0xF
    ret

.align 11
.global exception_table
exception_table:

    // 0x000 - 0x200
    
    // synchronous
    b   handle_unknown
    .align 7

    // IRQ
    b   handle_unknown
    .align 7

    // FIQ
    b   handle_unknown
    .align 7

    // SError
    b   handle_unknown
    .align 7

    // 0x200 - 0x400 // 

    // synchronous
    b   handle_unknown
    //b   exception_entry
    .align 7

    // IRQ
    b   handle_unknown
    //b   irq_entry
    .align 7

    // FIQ
    b   handle_unknown
    .align 7

    // SError
    b   handle_unknown
    .align 7

    // 0x400 - 0x600

     // synchronous
    b   exception_entry
    .align 7

    // IRQ
    //b   handle_unknown
    b   irq_entry
    .align 7

    // FIQ
    b   handle_unknown
    .align 7

    // SError
    b   handle_unknown
    .align 7

    // synchronous
    b   handle_unknown
    .align 7

    // IRQ
    b   handle_unknown
    .align 7

    // FIQ
    b   handle_unknown
    .align 7

    // SError
    b   handle_unknown
    .align 7 
