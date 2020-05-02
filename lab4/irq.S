.section ".text"


.global enable_interrupt
enable_interrupt:
	msr 	DAIFClr, 0xf
	ret

.global disable_interrupt
disable_interrupt:
	msr 	DAIFSet, 0xf
	ret

.global enable_EL0_interrupt
enable_EL0_interrupt:
	mrs 	x0, spsr_el1
	ldr 	x1, =0xFC3F
	and 	x0, x0, x1
	msr 	spsr_el1, x0 	
	ret

.global disable_EL0_interrupt
disable_EL0_interrupt:
	mrs 	x0, spsr_el1
	ldr 	x1, =0x03C0
	orr 	x0, x0, x1
	msr 	spsr_el1, x0
	ret

.global brk_instr
brk_instr:
	brk 	#1
	ret

.global get_sync_exc_param_el1
get_sync_exc_param_el1:
	mrs 	x9, esr_el1
	mrs 	x10, elr_el1
	str 	x9, [x0]
	str 	x10, [x1]
	ret

.global get_sync_exc_param_el2
get_sync_exc_param_el2:
	mrs 	x9, esr_el2
	mrs 	x10, elr_el2
	str 	x9, [x0]
	str 	x10, [x1]
	ret

.global sleep
sleep:
	ldr 	x0, =0x1FFFFFF
loop:
	sub 	x0, x0, 1
	cbnz 	x0, loop
	ret

.global get_taskId
get_taskId:
	mov 	x8, 0
	svc 	0
	ret

.global sched_yield
sched_yield:
	mov 	x8, 1
	svc 	0
	ret

.global uart_write
uart_write:
	mov 	x8, 2
	mov 	x9, x0
	svc 	0
	ret

.global uart_read
uart_read:
	mov 	x8, 3
	mov 	x9, x0
	mov 	x10, x1
	svc 	0
	ret

.global exec
exec:
	mov 	x8, 4
	mov 	x9, x0
	svc 	0
	ret

.global fork
fork:
	mov 	x8, 5
	svc 	0
	ret

.global exit
exit:
	mov 	x8, 6
	mov 	x9, x0
	svc 	0
	ret