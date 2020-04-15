.section ".text.assembly"
.global _start
.align 11
.global exception_table
.global save_all
.global load_all
.global from_el1_to_el0
.global from_el2_to_el1
.equ USER_STACK, 0x1000
_start:
	mrs	x0, mpidr_el1
	and	x0, x0, 3
	cbz	x0, set_stack
busy:
	wfe
	b	busy
set_stack:
	ldr	x0, = _stack_top
	mov	sp, x0
clean_bss:
	ldr	x1, =__bss_start
	ldr	x2, =__bss_size
loop:	
	cbz	x2, set_exception_table
	str	xzr, [x1], #8
	sub	x2, x2, #1
	cbnz	x2, loop
set_exception_table:
	ldr 	x0, =exception_table
	msr 	VBAR_EL2, x0
start_c:
	bl	main
	b	busy
from_el2_to_el1:
  	mov 	x0, (1 << 31) // EL1 use aarch64
  	msr 	hcr_el2, x0
  	mov 	x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
  	msr 	spsr_el2, x0
  	adr 	x0, rest_initialization // load exception return address
  	msr 	elr_el2, x0
  	adr 	x0, _stack_top // init sp for el1 option 1
  	msr 	sp_el1, x0
	ldr 	x0, =exception_table
	msr 	VBAR_EL1, x0
  	eret // return to EL1
rest_initialization:
  	#adr 	x0, _stack_top // init sp for el1 option 2
  	mov 	sp, x0

save_all:
  	sub sp, sp, 32 * 8
  	stp x0, x1, [sp ,16 * 0]
  	stp x2, x3, [sp ,16 * 1]
  	stp x4, x5, [sp ,16 * 2]
  	stp x6, x7, [sp ,16 * 3]
  	stp x8, x9, [sp ,16 * 4]
  	stp x10, x11, [sp ,16 * 5]
  	stp x12, x13, [sp ,16 * 6]
  	stp x14, x15, [sp ,16 * 7]
  	stp x16, x17, [sp ,16 * 8]
  	stp x18, x19, [sp ,16 * 9]
  	stp x20, x21, [sp ,16 * 10]
  	stp x22, x23, [sp ,16 * 11]
  	stp x24, x25, [sp ,16 * 12]
  	stp x26, x27, [sp ,16 * 13]
  	stp x28, x29, [sp ,16 * 14]
  	str x30, [sp, 16 * 15]
	ret
from_el1_to_el0:
	mov 	x0, USER_STACK
	msr	sp_el0, x0
	mov	x0, 0
	msr	spsr_el1, x0
	adr	x0, shell
//	adr	x0, rest_initialization
	msr	elr_el1, x0
	eret
load_all:
  	ldp x0, x1, [sp ,16 * 0]
  	ldp x2, x3, [sp ,16 * 1]
  	ldp x4, x5, [sp ,16 * 2]
  	ldp x6, x7, [sp ,16 * 3]
  	ldp x8, x9, [sp ,16 * 4]
  	ldp x10, x11, [sp ,16 * 5]
  	ldp x12, x13, [sp ,16 * 6]
  	ldp x14, x15, [sp ,16 * 7]
  	ldp x16, x17, [sp ,16 * 8]
  	ldp x18, x19, [sp ,16 * 9]
  	ldp x20, x21, [sp ,16 * 10]
  	ldp x22, x23, [sp ,16 * 11]
  	ldp x24, x25, [sp ,16 * 12]
  	ldp x26, x27, [sp ,16 * 13]
  	ldp x28, x29, [sp ,16 * 14]
  	ldr x30, [sp, 16 * 15]
  	add sp, sp, 32 * 8
	eret 
.align 11
exception_table:
	.align 7
  	bl exception_handler0 
  	.align 7 
  	bl exception_handler1
  	.align 7
  	bl exception_handler2
  	.align 7
  	bl exception_handler3
  	.align 7
 	bl exception_handler4
	eret
  	.align 7
 	b exception_handler5
 	.align 7
 	bl exception_handler6
 	.align 7
  	bl exception_handler7
 	.align 7
 	bl exception_handler8
 	.align 7
 	bl exception_handler9
 	.align 7
 	bl exception_handlerA
 	.align 7
 	bl exception_handlerB
 	.align 7
 	bl exception_handlerC
 	.align 7
 	bl exception_handlerD
 	.align 7
 	bl exception_handlerE
 	.align 7
 	bl exception_handlerF
  	.align 7

