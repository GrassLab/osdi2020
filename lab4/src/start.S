_start:
    mrs x0, mpidr_el1
    and x0, x0, 3
    cbz x0, 2f
1:
    wfe
    b 1b
2:
    ldr x0, = __bss_start
    ldr x1, = __bss_end

init_bss:
    str xzr, [x0]
    add x0, x0, #8
    cmp x0, x1
    ble init_bss

// load exception_table to VBAR_EL1
    ldr x0, =exception_table
    msr VBAR_EL1, x0
// from el2 to el1
    mov x0, (1 << 31)
    msr HCR_EL2, x0
    mov x0, 0x3c5
    msr SPSR_EL2, x0
    adr x0, kernel_init
    msr ELR_EL2, x0
    ldr x0, =_start
    msr SP_EL1, x0
    eret

/*from_el1_to_el0:
    mov x0, 0x1000
    msr SP_EL0, x0
    mov x0, 0 // EL0 with interrupt enabled
    msr SPSR_EL1, x0
    adr x0, main // return to shell run in EL0
    msr elr_el1, x0
    msr DAIFClr, 0xf
    eret*/

1:
    b 1b
