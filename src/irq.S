.globl enableIrq
enableIrq:
    mrs x0, HCR_EL2
    orr x0, x0, #16
    msr HCR_EL2, x0
    msr DAIFClr, #0xf
    ret

.globl disableIrq
disableIrq:
    msr DAIFSet, #0xf
    ret