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
    mrs     x1,     SPSR_EL1
    ldr     x2,     =0xFC3F
    and     x1,     x1,         x2
    msr     SPSR_EL1, x1

    ret

# irq disable in EL1
irq_el1_disable:

    mrs     x1,     SPSR_EL1
    orr     x1,     x1,         0x3c0
    msr     SPSR_EL1, x1

    ret