.section ".text"

.global irq_el1_enable
.global irq_el1_disable

.global irq_el2_setup
.global irq_el2_enable
.global irq_el2_disable

# irq enable in EL1
irq_el1_enable:

    # Approach 1
    mov x1, #0b1111000000
    msr DAIF, x1

    # Approach 2
    # msr DAIFClr, 0xf

    # Approach 3
    # EL0 with interrupt enabled
    # SPSR_EL1: DAIF[4]: SPSR_EL1[9:6]
    # mov x1, #0
    # mrs x1, SPSR_EL1
    # and x1, x1, 0b00001111111
    # msr SPSR_EL1, x1
    ret

# irq disable in EL1
irq_el1_disable:

    # Approach 1
    msr DAIFSet, 0xf

    # Approach 2
    # mov x1, #0x60000000
    # mrs x1, SPSR_EL1
    # orr x1, x1, 0b11110000000
    # msr SPSR_EL1, x1
    ret

# irq setup in EL2
irq_el2_setup:

    # irq init (HCR_EL2.IMO
    mrs x1, HCR_EL2
    orr x1, x1, #0b10000
    msr HCR_EL2, x1

    # return
    ret

# irq enable in EL2
irq_el2_enable:
    msr DAIF, xzr
    ret

# irq disable in EL2
irq_el2_disable:
    mov x1, #0b1111000000
    msr DAIF, x1
    ret