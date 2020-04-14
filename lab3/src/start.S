
#define PAGE_SHIFT	 		12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)	
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define EL1_LOW_MEMORY              	(2 * SECTION_SIZE)
#define EL0_LOW_MEMORY              (4 * SECTION_SIZE)

.section ".text.boot"

.global _start
    
_start:
    mrs    x0, mpidr_el1
    and    x0, x0, #3 
    cbz    x0, master //if x0 == 0 jump to forward of set_stack_bss
    b      loop

loop:  
    wfe
    b      loop //busy loop


master:
    adr    x0, _start
    msr    sp_el1, x0
    # set RW (AArch64) The Execution state for EL1 is AArch64 When RW == 0b1
    mov    x0, #(1 << 31)
    msr    hcr_el2, x0

    # switch to EL1
    mov    x0, ((7 << 6) | (5 << 0))
    msr    spsr_el2, x0
    mov    x0, #0
    msr    DAIF, x0

    adr    x0, el1_start
    msr    elr_el2, x0
    eret

el1_start:
    adr	x0, __bss_start
	adr	x1, __bss_end
	sub	x1, x1, x0
	bl 	memzero
	mov	sp, #EL1_LOW_MEMORY

    ldr    x0, =exception_table
    msr    VBAR_EL1, x0
    
    bl enable_irq
    ldr x0, ((1 << 6) & ~(15 << 0))
	msr spsr_el1, x0

    adr	x0, el0_start		
	msr	elr_el1, x0
    eret

el0_start:
    mov	sp, #EL0_LOW_MEMORY
    bl	main
	b 	loop	


.globl memzero
memzero:
	str xzr, [x0], #8
	subs x1, x1, #8
	b.gt memzero
	ret