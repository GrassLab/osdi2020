.section ".text"

.global enable_interrupt
.global disable_interrupt
.global supervisor_call_1
.global supervisor_call_2
.global supervisor_call_3
.global supervisor_call_4
.global brk_instr
.global get_sync_exc_param_el1



enable_interrupt:
	mrs 	x0, spsr_el1
	ldr 	x1, =0xFC3F
	and 	x0, x0, x1
	msr 	spsr_el1, x0 	
	ret

disable_interrupt:
	mrs 	x0, spsr_el1
	ldr 	x1, =0x03C0
	orr 	x0, x0, x1
	msr 	spsr_el1, x0
	ret

supervisor_call_1:
	svc 	1
	ret

supervisor_call_2:
	svc 	2
	ret

supervisor_call_3:
	svc 	3
	ret

supervisor_call_4:
	svc 	4
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

