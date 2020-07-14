.globl irq_vector_init
irq_vector_init:
	adr	x0, exception_table // load VBAR_EL1 with virtual
	msr	vbar_el1, x0	// vector table address
	ret

.global enable_irq
enable_irq:
	msr    daifclr, #0x2  //clear IRQ mask bit 
	ret

.global disable_irq
disable_irq:
	msr	daifset, #0x2
	ret

.global async_exc_routing
async_exc_routing:
	mrs x0,hcr_el2
        //orr x0,x0,#(1<<5) //AMO for SError
        orr x0,x0,#(1<<4) //IMO for IRQ
        //orr x0,x0,#(1<<3) //FMO for FIQ
        msr hcr_el2,x0
        ret
