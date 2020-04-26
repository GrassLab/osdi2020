.section ".text.boot"

.global _start

_start:
    // load additional PE id in GPR x0
    mrs x0, mpidr_el1
    // filter the cpuid, only cpuid=0 can pass in next instr
    // other cpuid (1, 2, 3)'s x0 won't be 0 after and
    and x0, x0, #3
    // compare and branch on Zero
    cbz x0, master

hang:
    wfe
    b hang

master:
    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl memzero

    mov sp, #0x400000

    // setup exception table for EL1
    ldr x0, =_exception_table
    msr VBAR_EL1, x0

    // setup HCR_EL2.RW to 1 bc we're running 64 bit kernel
    mrs x0, HCR_EL2
    mov x1, 1 << 31 // [31] RW @ HCR_EL2
    orr x0, x0, x1
    msr HCR_EL2, x0

    // setup lower sp_eln
    add x0, sp, #0x2000
    msr SP_EL0, x0
    add x0, x0, #0x2000
    msr SP_EL1, x0

    b _from_el2_to_el1

_from_el2_to_el1:
    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr SPSR_EL2, x0
    adr x0, kernel_main // load exception return address
    msr ELR_EL2, x0
    eret // return in EL1

_from_el1_to_el0:
    mov x0, 0 // EL0 with interrupt enabled
    msr SPSR_EL1, x0
    adr x0, kernel_main
    msr ELR_EL1, x0
    eret // return to kernel_main and run in EL0
