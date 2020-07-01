.global el1_to_el0
el1_to_el0:
    msr ELR_EL1, x0
    msr SP_EL0, x1
    //mov x0, 0 // EL0 with interrupt enabled
    msr SPSR_EL1, xzr
    //adr x0, main // return to shell run in EL0
    //msr DAIFClr, 0xf
    eret
