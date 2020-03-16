.section ".text.boot"

.global _start

_start:
	mrs		x1, mpidr_el1 			//move to arm register form system coprocessor register
	and 	x1, x1, #3				
	cbz 	x1, init 				//compare, if 0 then jump. if cpu-0 jump to init. others go to busy_loop

busy_loop: 	
	wfe								//wait for event
	b 		busy_loop

init: 	
	ldr 	x1, =stack_top			//set stack pointer
	mov 	sp, x1

	ldr 	x1, =__bss_start		//set bss segment
	ldr 	w2, =__bss_size

clear_bss:
	cbz 	w2, c_entry				//if .bss size is zero, jump to c_entry, else clear bss
	str 	xzr, [x1], #8			//store register. xzr is zero register. *x1 = 0; x1 += 8;
	sub 	w2, w2, #1				//w2 = w2 - 1;
	cbnz 	w2, clear_bss			//if w2>0 jump clear_bss. [ LOOP EXECUTE ]

c_entry:
	bl		main
	b 		busy_loop

