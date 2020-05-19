#include "mmu.h"

/*
	NOTE:

	LDR r,  label : will store the "value" of label to register r
	LDR r, =label : will stroe the "address" of the label to register r. BTW which is PC-relative address

	_kernel_end is defined in linker script. it is not an object (it doesn't keep in memory)
	TCP_CONFIG_DEFAULT is defined in header file. Header file in assembly is an object. (but it just plain text in C file)
*/
.global mmu_init
mmu_init:
	ldr x0, = TCR_CONFIG_DEFAULT
	msr tcr_el1, x0

	ldr x0, = MAIR_DEFAULT
	msr mair_el1, x0

	ldr x0, _kernel_end 	// PGD @ _kernel_end
	add x1, x0, 0x1000		// PUD @ _kernel_end + 1 page

	ldr x2, = BOOT_PGD_ATTR
	orr x2, x1, x2 // combine the physical address of next level page with attribute.
	str x2, [x0]

	ldr x2, = BOOT_PUD_ATTR
	mov x3, 0x00000000 // 1GB
	orr x3, x2, x3
	str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
	mov x3, 0x40000000
	orr x3, x2, x3
	str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

	msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
	msr ttbr1_el1, x0 // also load PGD to the upper translation based register.

	ret	