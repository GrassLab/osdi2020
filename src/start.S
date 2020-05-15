.section ".text"

#include "memory/mmConfig.h"

.global _start

_start:
    // let core with cpuid != 0 enter busy loop
    mrs x0, mpidr_el1
    and x0, x0, #3
    cbz x0, meminit
busyloop:
    wfe
    b busyloop
meminit:
    ldr x0, = TCR_CONFIG_DEFAULT
    msr tcr_el1, x0

    ldr x0, =( \
        (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
        (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0

    mov x0, 0 // PGD's page frame at 0x0
    mov x1, 0x1000 // PUD's page frame at 0x1000

    ldr x2, = BOOT_PGD_ATTR
    orr x2, x1, x2 // combine the physical address of next level page with attribute.
    str x2, [x0]

    ldr x2, = BOOT_PUD_ATTR
    mov x3, 0x00000000
    orr x3, x2, x3
    str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
    mov x3, 0x40000000
    orr x3, x2, x3
    str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

    msr ttbr0_el1, x0 // load PGD to the buttom translation based register.
    msr ttbr1_el1, x0 // also load PGD to the upper translation based register.

    mrs x2, sctlr_el1
    orr x2 , x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled

    // clear the .bss segment to all zeros
    adr x1, __bss_start
    adr x2, __bss_end
    mov x3, #0
bssloop:
    cmp x1, x2
    bge from_el2_to_el1
    str x3, [x1], #8
    b bssloop
from_el2_to_el1:
    bl irqVectorInit
    mov x0, (1 << 31) // EL1 use aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt enable (0x5 enable) (0x3c5 disable)
    msr spsr_el2, x0
    adr x0, shell // load exception return address
    msr elr_el2, x0
    adr x0, __stack_start // init sp for el1 option 1
    msr sp_el1, x0
    eret // return to EL1
// from_el1_to_el0:   
//    mov x0, 0x1000
//    msr sp_el0, x0
//    mov x0, 0 // EL0 with interrupt enabled
//    msr spsr_el1, x0
//    adr x0, shell // return to shell run in EL0
//    msr elr_el1, x0
//    eret
shell:
    ;ldr x0, = __stack_start
    ;mov sp, x0

    ldr x0, =main
    br x0
    //bl main
    b busyloop