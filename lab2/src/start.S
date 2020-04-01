.section ".text"
_start:
    //read cpu id
	mrs x0, mpidr_el1   //Move to ARM register from system coprocessor register
	and x0, x0, 3
	cbz x0, 2f

// let core with cpu id != 0 enter busy loop
1:
	wfe
	b _start

//cpu id == 0
2:
	// set stack
	ldr x1, =_start
	mov sp, x1

        // clear bss
	ldr x1, =__bss_start
	ldr w2, =__bss_size
3:
	cbz w2, 4f
	str xzr, [x1], 8 //fill zero 
	sub w2, w2, 1
	cbnz w2, 3b

// jump to main function
4:     
	bl main
	b 1b
