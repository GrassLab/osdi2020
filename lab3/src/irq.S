.globl enable_irq
enable_irq:
	msr    daifclr, #2 
	mov    x0, #16
	msr    hcr_el2, x0
	ret

.globl disable_irq
disable_irq:
	msr	daifset, #2
	ret