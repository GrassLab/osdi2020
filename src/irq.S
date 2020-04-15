.globl enable_irq
enable_irq:
    msr DAIFClr, #0xf
    ret
    //mrs x0, HCR_EL2
    //orr x0, x0, #16
    //msr HCR_EL2, x0
    

.globl disable_irq
disable_irq:
    msr DAIFSet, #0xf
    ret
