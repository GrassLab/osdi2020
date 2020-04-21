.global enable_irq
enable_irq:
	msr    daifclr, #2  //clear IRQ mask bit 
	ret

.global disable_irq
disable_irq:
	msr	daifset, #2
	ret

.global async_exc_routing
async_exc_routing:
	mrs x0,hcr_el2
        //orr x0,x0,#(1<<5) //AMO for SError
        orr x0,x0,#(1<<4) //IMO for IRQ
        //orr x0,x0,#(1<<3) //FMO for FIQ
        msr hcr_el2,x0
        ret
