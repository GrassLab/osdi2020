.global _irq_init
_irq_init:
    msr DAIFclr, #0xf
    mrs x0, hcr_el2
    orr x0, x0, #16
    msr hcr_el2, x0
    
    ret

.global _irq_end
_irq_end:
    msr DAIFset, #0xf
    ret
