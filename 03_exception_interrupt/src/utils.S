.global enable_interrupt_el2
enable_interrupt_el2:
    mrs x9, hcr_el2
    orr x9, x9, #(7 << 3)
    orr x9, x9, #(1 << 27)
    msr hcr_el2, x9
    msr daifclr, 0xf
    ret

.global init_for_el1
init_for_el1:
    mrs x0, hcr_el2
    orr x0, x0, #(1<<31)
    msr hcr_el2, x0
    ldr x0, =exception_table_el1
    msr VBAR_EL1, x0
    ret

// load exception_table to VBAR_EL2
.globl init_el2_vector
init_el2_vector:
    ldr x0, =exception_table_el2
    msr VBAR_EL2, x0
    ret

.globl get_el
get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret
