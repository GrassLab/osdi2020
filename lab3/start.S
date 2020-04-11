.section ".text.boot"



.global _start

_start:
	mrs		x1, mpidr_el1
	and 	x1, x1, #3
	cbz		x1, 2f
	// load exception_table to VBAR_EL2
	ldr x0, =exception_table
	msr VBAR_EL2, x0

1: 
	wfe
	b		1b

2:
	ldr		x1, =_start
	mov		sp, x1
	
	// clear bss
	ldr		x1, =__bss_start
	ldr 	w1, =__bss_size

3:
	cbz		w2, 4f
	str		xzr, [x1], #8
	sub		w2, w2, #1
	cbnz	w2, 3b

	// jump to C code
4:
	bl		irq_vector_init
	bl 		main
	b		1b

.globl irq_vector_init
irq_vector_init:
	adr	x0, exception_table		// load VBAR_EL1 with virtual
	msr	vbar_el2, x0		// vector table address
	ret


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

    b sync_currentEL_SP_ELx
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
    b exception_handler
    .align 7

exception_handler:
	bl el2_not_implement
    b proc_hang

sync_currentEL_SP_ELx:
    mrs x0, ELR_EL2
    mrs x1, ESR_EL2
    bl el2_current_sp2_sync_message
	eret

proc_hang:
    b proc_hang
