.section ".text"

.global irq_setup
.global irq_enable
.global irq_disable

irq_setup:

    # irq init (HCR_EL2.IMO
    mrs x0, hcr_el2
    orr x0, x0, #0b10000
    msr hcr_el2, x0

    # return
    ret

irq_enable:
    msr daif, xzr
    ret

irq_disable:
    mov x0, #0b1111000000
    msr daif, x0
    ret