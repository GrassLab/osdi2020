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
