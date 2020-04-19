//.global irq_init
irq_init:
    mrs     x0, hcr_el2     // hypervisor config register
    orr     x0, x0, 0x10    // set IMO bit, physical irq route to el2
    msr     hcr_el2, x0
    ret

.global irq_unmask
irq_unmask:
    msr     DAIFClr, 0xF    // clear interrupt mask
    ret

.global irq_mask
irq_mask:
    msr     DAIFSet, 0xF    // set interrupt mask
    ret

