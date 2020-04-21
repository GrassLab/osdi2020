.section ".text.boot"
#define USER_STACK 0x1000
.global _start

_start:
	mrs x1, mpidr_el1
	and x1, x1, #3		//cpuid
	cbz x1, _core0
_busy_loop:	
	wfe	
	b _busy_loop
_core0:		
	ldr x1, =_bss_start
	ldr w2, =_bss_size
_clear_bss:	
	cbz w2, from_el2_to_el1
	str xzr, [x1], #8
	sub w2, w2, #1
	cbnz w2, _clear_bss
from_el2_to_el1:
	mov x0, #(1 << 31) 			// EL1 use aarch64
	msr hcr_el2, x0
	mov x0, #0x3c5 				// EL1h (SPSel = 1) with interrupt disabled
	msr spsr_el2, x0
	adr x0, rest_init 			// load exception return address
	msr elr_el2, x0
	adr x0, _start	 			// init sp for el1 option 1
	msr sp_el1, x0
	eret 						// return to EL1
rest_init:
	adr x0, _start
	mov sp, x0
	mrs x0, cpacr_el1
	orr x0, x0, #(3 << 20)
	msr cpacr_el1, x0
	ldr x0, =exception_table_el1    //set exception table
	msr vbar_el1, x0
	mov x0, 0x0
	msr spsr_el1, x0
from_el1_to_el0:
	mov x0, USER_STACK
	msr sp_el0, x0
	mov x0, 0 // EL0 with interrupt enabled
	msr spsr_el1, x0
	adr x0, _main // return to shell run in EL0
	msr elr_el1, x0
	eret
_main:
	bl main
