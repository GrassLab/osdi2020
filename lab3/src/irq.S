.globl irq_vector_init
irq_vector_init:
	adr	x0, vectors			// load VBAR_EL1 with virtual
	msr	vbar_el1, x0		// vector table address
	ret

.globl enable_irq
enable_irq:
	// D: Debug mask bit
	// A: SError mask bit
	// I: IRQ mask bit
	// F: FIQ mask bit
	// set "I" field to 1 (DAIF, 0b0010) which means enabling of IRQ interrupt
	msr	daifclr, #2 
	ret

.globl disable_irq
disable_irq:
	// D: Debug mask bit
	// A: SError mask bit
	// I: IRQ mask bit
	// F: FIQ mask bit
	// set "I" field to 1 (DAIF, 0b0010) which means disabling of IRQ interrupt
	msr	daifset, #2
	ret

.global	svc_call_1
svc_call_1:
	svc	#1	// no use QQ
