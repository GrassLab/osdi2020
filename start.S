.section ".text.boot"

.global _start

_start:
	mrs x1, mpidr_el1
	and x1, x1, #3		//cpuid
	cbz x1, _core0
_busy_loop:	
	wfe	
	b _busy_loop
_core0:		
	ldr x1, =_start
	mov sp, x1		//set sp
	ldr x1, =_bss_start
	ldr w2, =_bss_size
_clear_bss:	
	cbz w2, _main
	str xzr, [x1], #8
	sub w2, w2, #1
	cbnz w2, _clear_bss
_main:	
	bl main
