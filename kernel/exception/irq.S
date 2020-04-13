.section ".text"

.global irq_el1_enable
.global irq_el1_disable

.global irq_el2_setup
.global irq_el2_enable
.global irq_el2_disable

# irq enable in EL1
irq_el1_enable:
    
    # EL0 with interrupt enabled
    # SPSR_EL1: DAIF[4]: SPSR_EL1[9:6]
    mov x0, #0
    # mrs x0, SPSR_EL1
    # and x0, x0, 0b00001111111
    msr SPSR_EL1, x0
    ret

# irq disable in EL1
irq_el1_disable:
    mrs x0, SPSR_EL1
    orr x0, x0, 0b11110000000
    msr SPSR_EL1, x0
    ret

# irq setup in EL2
irq_el2_setup:

    # irq init (HCR_EL2.IMO
    mrs x0, HCR_EL2
    orr x0, x0, #0b10000
    msr HCR_EL2, x0

    # return
    ret

# irq enable in EL2
irq_el2_enable:
    msr DAIF, xzr
    ret

# irq disable in EL2
irq_el2_disable:
    mov x0, #0b1111000000
    msr DAIF, x0
    ret