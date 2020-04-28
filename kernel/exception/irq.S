.section ".text"

.global irq_el1_enable
.global irq_el1_disable

.global irq_el2_setup
.global irq_el2_enable
.global irq_el2_disable

# irq enable in EL1
irq_el1_enable:

    # Approach 1: This is not an appropriate approach, 
    # this would copy to the daif field in the exception level, 
    # which trap into the current el.
    # mov x1, #0b1111000000
    # msr DAIF, x1

    # Approach 2
    # msr DAIFClr, 0xf

    # Approach 3
    # EL0 with interrupt enabled
    # SPSR_EL1: DAIF[4]: SPSR_EL1[9:6]
    mrs     x1,     SPSR_EL1
    # 0b1111110000111111‬
    ldr     x2,     =0xFC3F
    and     x1,     x1,         x2
    msr     SPSR_EL1, x1

    ret

# irq disable in EL1
irq_el1_disable:

    # Approach 1  # This is not an appropriate approach, 
    # msr DAIFSet, 0xf

    # Approach 2
    mrs     x1,     SPSR_EL1
    orr     x1,     x1,         0x3c0
    msr     SPSR_EL1, x1

    ret

# irq setup in EL2
irq_el2_setup:

    # irq init (HCR_EL2.IMO
    mrs     x1,     HCR_EL2
    orr     x1,     x1,         #0b10000
    msr     HCR_EL2, x1

    # return
    ret

# irq enable in EL2
irq_el2_enable:
    msr     DAIF,   xzr

    ret

# irq disable in EL2
irq_el2_disable:
    mov     x1,     #0b1111000000
    msr     DAIF,   x1
    
    ret