#define __ASSEMBLY__
#include "sysregs.h"
#include "mmu.h"
#include "mm.h"

.section ".text.boot"

.global _start

_start:
    // get cpu id
    mrs     x1, MPIDR_EL1
    and     x1, x1, #3
    cbz     x1, 2f
    // if cpu_id > 0, stop
1:
    wfe
    b       1b
    // if cpu_id == 0
2:
    // disable MMU
    ldr     x1, =SCTLR_VALUE_MMU_DISABLED
    msr     sctlr_el1, x1

    // make el0, el1 can use Floating point and Advanced SIMD
    ldr     x1, =CPACR_EL1_VALUE
    msr     CPACR_EL1, x1

    // set AArch64 for el2
    ldr     x1, =HCR_EL2_VALUE
    msr     hcr_el2, x1

    // mask all interrupt, and set interrupt level to el1h
    ldr     x1, =SPSR_EL2_VALUE
    msr     spsr_el2, x1

    adr     x1, el1_start
    msr     elr_el2, x1

    eret

el1_start:
    // load exception_table to VBAR_EL1
    adr     x1, exception_table
    msr     vbar_el1, x1

    // mask all interrupt, and set exception level to el0
    ldr     x1, =SPSR_EL1_VALUE
    msr     spsr_el1, x1

    // set stack pointer
    ldr     x1, =0x60000
    mov     sp, x1

    // setup tcr
    ldr     x0, =TCR_EL1_VALUE
    msr     tcr_el1, x0

    // setup mair
    ldr     x0, =MAIR_VALUE
    msr     mair_el1, x0

    // create page table
    bl      page_table_create

    // enable MMU
    ldr     x1, =SCTLR_VALUE_MMU_ENABLE
    mrs     x2, sctlr_el1
    orr     x2, x2, x1
    msr     sctlr_el1, x2

    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b
4:
    // mov sp to virtual address
    ldr     x1, =KERNEL_VIRT_BASE
    add     sp, sp, x1

    // indirect branch
    ldr     x0, =boot_init
    br      x0

1:
    b       1b

    adr     x1, el0_start
    msr     elr_el1, x1

    eret

el0_start:
    // set stack pointer
    ldr     x1, =0x40000
    mov     sp, x1

    // jump to main function in C
    bl      main
    // halt this core if return
    b       1b

/* ------- */

page_table_create:
    /*
     * Each page table base address:
     *  x0: PGD (1 page, 1 entry)
     *  x1: PUD (1 page, 2 entry)
     *  x2: PMD (1 page, 512 entry)
     *  x3: PTE (512 page, each with 512 entry)
     */
    ldr     x0, =pg_dir
    lsl     x0, x0, #16  // omit first 16 bit (using physical address)
    lsr     x0, x0, #16
    add     x1, x0, #PAGE_SIZE
    add     x2, x1, #PAGE_SIZE
    add     x3, x2, #PAGE_SIZE

    // setup PGD
    ldr     x4, =PGD0_ATTR
    orr     x4, x1, x4 // PUD physical address
    str     x4, [x0]

    // setup PUD1
    ldr     x4, =PUD0_ATTR
    orr     x4, x2, x4 // PMD physical address
    str     x4, [x1]

    // setup PUD2 (1GB section start from 0x40000000)
    ldr     x4, =PUD1_ATTR
    mov     x5, 0x40000000
    orr     x4, x5, x4
    str     x4, [x1, #8]

    // setup PMD (512 entry in 1 page)
    mov     x4, x3 // point to current PTE address
    mov     x5, xzr // i = 0
    mov     x6, #512
1:
    ldr     x7, =PMD0_ATTR
    orr     x7, x4, x7 // PTE physical address
    str     x7, [x2, x5, lsl #3] // (i * 8)
    add     x5, x5, #1
    add     x4, x4, #PAGE_SIZE
    cmp     x5, x6
    b.ls    1b

    // setup normal PTE (512 * 512 - 4096 = 258048 entry)
    mov     x4, xzr // physical address
    mov     x5, xzr // i = 0
    mov     x6, #258048
2:
    ldr     x7, =PTE_RAM_ATTR
    orr     x7, x4, x7
    str     x7, [x3, x5, lsl #3] // (i * 8)
    add     x5, x5, #1
    add     x4, x4, #PAGE_SIZE
    cmp     x5, x6
    b.ls    2b

    // setup device PTE (16MB = 4096 entry)
    add     x6, x6, #4096 // 262144
3:
    ldr     x7, =PTE_MMIO_ATTR
    orr     x7, x4, x7
    str     x7, [x3, x5, lsl #3] // (i * 8)
    add     x5, x5, #1
    add     x4, x4, #PAGE_SIZE
    cmp     x5, x6
    b.ls    3b

    msr     ttbr0_el1, x0 // load PGD to the buttom translation based register.
    msr     ttbr1_el1, x0 // load PGD to the upper translation based register.
    
    ret
