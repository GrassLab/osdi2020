.section ".text"

.global enable_interrupt
.global disable_interrupt
.global enable_EL0_interrupt
.global disable_EL0_interrupt
.global brk_instr
.global get_sync_exc_param_el1
.global sleep

enable_interrupt:
	msr 	DAIFClr, 0xf
	ret

disable_interrupt:
	msr 	DAIFSet, 0xf
	ret

enable_EL0_interrupt:
	mrs 	x0, spsr_el1
	ldr 	x1, =0xFC3F
	and 	x0, x0, x1
	msr 	spsr_el1, x0 	
	ret

disable_EL0_interrupt:
	mrs 	x0, spsr_el1
	ldr 	x1, =0x03C0
	orr 	x0, x0, x1
	msr 	spsr_el1, x0
	ret

brk_instr:
	brk 	#1
	ret

get_sync_exc_param_el1:
	sub     sp, sp, #8 * 3
	str 	x2, [sp, #16]
	str 	x3, [sp, #8]
	mrs 	x2, esr_el1
	mrs 	x3, elr_el1
	str 	x2, [x0]
	str 	x3, [x1]
	ldr 	x2, [sp, #16]
	ldr 	x3, [sp, #8]
	add  	sp, sp, #8 * 3
	ret

sleep:
	ldr 	x0, =0x1FFFFFF
loop:
	sub 	x0, x0, 1
	cbnz 	x0, loop
	ret


.macro SVC_DECLARE svcNum
.global supervisor_call_\svcNum
supervisor_call_\svcNum :
	svc 	\svcNum
	ret
.endm

SVC_DECLARE 1
SVC_DECLARE 2
SVC_DECLARE 3
SVC_DECLARE 4
SVC_DECLARE 5
SVC_DECLARE 6
