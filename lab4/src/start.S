.section ".text.boot"

#define STACK_SIZE 0x1000

.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    // cpu id > 0, stop
1:  wfe
    b       1b

2:  // switch to el1
    ldr     x1, =_start
    msr     sp_el1, x1
    // Setup SCTLR access
    mov     x2, #0x0800
    movk    x2, #0x30d0, lsl #16
    msr     sctlr_el1, x2
    // enable AArch64 in EL1
    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    // disable FP/NEON trap
    mrs    x2, cpacr_el1
    mov    x0, #(3 << 20)
    orr    x0, x2, x0
    msr    cpacr_el1, x0
    // set up exception handlers
    ldr     x2, =vector_table
    msr     vbar_el1, x2
    // change execution level to EL1
    mov     x2, #0x5          // EL1h and clear DAIF
    msr     spsr_el2, x2
    adr     x2, 5f
    msr     elr_el2, x2
    eret

/*5:  // switch to el0
    adr x0, _start
    sub x0, x0, #STACK_SIZE
    msr sp_el0, x0
    mov x0, 0x0
    msr spsr_el1, x0
    adr x0, 5f
    msr elr_el1, x0
    eret                 */

5:    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

    // jump to C code, should not return
4:  bl      main
    // for failsafe, halt this core too
    b       1b