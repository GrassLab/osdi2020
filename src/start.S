.section ".text"

_start:
    // let core with cpuid != 0 enter busy loop
    mrs x0, mpidr_el1
    and x0, x0, #3
    cbz x0, meminit
busyloop:
    wfe
    b busyloop
meminit:
    // clear the .bss segment to all zeros
    ldr x1, = __bss_start
    ldr x2, = __bss_end
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
    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0
    adr x0, from_el1_to_el0 // load exception return address
    msr elr_el2, x0
    adr x0, __stack_start // init sp for el1 option 1
    msr sp_el1, x0
    eret // return to EL1
from_el1_to_el0:   
    mov x0, 0x1000
    msr sp_el0, x0
    mov x0, 0 // EL0 with interrupt enabled
    msr spsr_el1, x0
    adr x0, shell // return to shell run in EL0
    msr elr_el1, x0
    eret
shell:
    ;ldr x0, = __stack_start
    ;mov sp, x0
    bl main
    b busyloop