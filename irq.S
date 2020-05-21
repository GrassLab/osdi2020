/*
.global irq_vector_init
irq_vector_init:
    adr x0, vectors     // load VBAR_EL1 with virtual
    msr vbar_el1, x0        // vector table address
    ret
*/

.global enable_irq
enable_irq:
    msr    daifclr, #2 
    ret

.global disable_irq
disable_irq:
    msr daifset, #2
    ret
