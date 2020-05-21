#include "mmu_size.h"

/*
	NOTE:

	LDR r,  label : will store the "value" of label to register r
	LDR r, =label : will stroe the "address" of the label to register r. BTW which is PC-relative address

	_kernel_end is defined in linker script. it is not an object (it doesn't keep in memory)
	TCP_CONFIG_DEFAULT is defined in header file. Header file in assembly is an object. (but it just plain text in C file)

	adr  : within +- 1MB of the current PC , must be aligned 16B
	adrp : within +- 4GB of the current PC , must be aligned 4KB
*/
.global mmu_init
mmu_init:
	ldr x0, = TCR_CONFIG_DEFAULT
	msr tcr_el1, x0

	ldr x0, = MAIR_DEFAULT
	msr mair_el1, x0

	// page table start
	adrp 	x0, _kernel_end				// x0 is PGD
	ldr 	x1, = PAGE_TABLE_SIZE
	add 	x2, x0, x1 					// x2 is PUD
	add 	x3, x2, x1 					// x3 is PMD

	// fill PGD entry
	ldr 	x4, = BOOT_PGD_ATTR
	orr 	x4, x2, x4
	str 	x4, [x0]

	// fill PUD entry
	ldr 	x4, = BOOT_PUD_ATTR
	orr 	x4, x3, x4
	str 	x4, [x2]

	//fill PMD entry
	mov 	x4, 0x0 
	ldr 	x5, = NORMAL_FRAME_NUM
	ldr 	x6, = BOOT_PMD_ATTR_NORMAL
	ldr 	x7, = KERNEL_FRAME_SIZE
NORMAL_FRAME_SET:
	orr 	x8, x4, x6
	str 	x8, [x3], #8
	add 	x4, x4, x7
	sub 	x5, x5, #1
	cbnz    x5, NORMAL_FRAME_SET

	ldr 	x5, = DEVICE_FRAME_NUM
	ldr 	x6, = BOOT_PMD_ATTR_DEVICE	
DEVICE_FRAME_SET:
	orr 	x8, x4, x6
	str 	x8, [x3], #8
	add 	x4, x4, x7
	sub 	x5, x5, #1
	cbnz    x5, DEVICE_FRAME_SET


	msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
	msr ttbr1_el1, x0 // also load PGD to the upper translation based register.

	ret	

.global copy_rd
copy_rd:
	ldr     x1, = _binary_user8_elf_start
    ldr     x2, = _binary_user8_elf_end
    sub 	x2, x2, x1
    lsr 	x2, x2, 3
loop:
	ldr 	x3, [x1]
	str 	x3, [x0]
	add 	x1, x1, 8
	add 	x0, x0, 8
	sub 	x2, x2, 1
	cbnz 	x2, loop

	mov 	x0, 0
	br 		x0
	ret

.global set_ttbr0
set_ttbr0:
	msr 	ttbr0_el1, x0
	ret