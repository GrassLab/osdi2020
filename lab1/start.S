.section ".text.assembly"
.global _start

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
	cbz	x2, start_c
	str	xzr, [x1], #8
	sub	x2, x2, #1
	cbnz	x2, loop
start_c:
	bl	main
	b	busy
