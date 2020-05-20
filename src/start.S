#include "mmu.h"

.section ".text.boot"

.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, el2_entry 
    // cpu id > 0, stop
_buzy_loop:
    wfe
    b       _buzy_loop
    
el2_entry:

    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    msr     sp_el1, x1
    // enable CNTP for EL1
    mrs     x0, cnthctl_el2
    orr     x0, x0, #3
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr
    // enable AArch64 in EL1
    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    // Setup SCTLR access
    mov     x2, #0x0800
    movk    x2, #0x30d0, lsl #16
    msr     sctlr_el1, x2
    // change execution level to EL1
    mov     x2, #0x344
    msr     spsr_el2, x2
    adr     x2, el1_entry
    msr     elr_el2, x2

    eret

el1_entry:  
    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    // set up exception handlers
    ldr     x2, =exception_table
    msr     vbar_el1, x2

    ldr     x0, 0
    msr     spsr_el1, x0

memory:
    ldr x0, =TCR_CONFIG_DEFAULT
    msr tcr_el1, x0

    ldr x0, =( \
      (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
      (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0

    ldr x0, =0x0// PGD's page frame at 0x0
    ldr x1, =0x1000// PUD's page frame at 0x1000
    ldr x2, =0x2000// 1st PMD's page frame at 0x2000
    ldr x3, =0x3000// 2nd PMD's page frame at 0x4000

    ldr x4, = BOOT_PGD_ATTR 
    orr x4, x1, x4 // combine the physical address of next level page with attribute.
    str x4, [x0]

    ldr x4, =BOOT_PUD_ATTR
    orr x4, x2, x4
    str x4, [x1] // 1st 1GB mapped by the 1st entry of PUD
    orr x4, x3, x4
    str x4, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

    ldr x4, =BOOT_PMD_NOR_ATTR
    mov x5, #512
    mov x6, xzr
    mov x7, x2
    
mem_loop_normal:
    mov x8, x6, lsl #21 // x8 = x6 << 20
    orr x8, x4, x8
    str x8, [x7], #8
    add x6, x6, #1
    cmp x6, x5
    b.ls mem_loop_normal

    ldr x4, =BOOT_PMD_NOR_ATTR
    mov x5, #512

mem_loop_device:
    mov x8, x6, lsl #21
    orr x8, x4, x8
    str x8, [x7], #8
    add x6, x6, #1
    cmp x6, x5
    b.ls mem_loop_device


    msr ttbr0_el1, x0
    msr ttbr1_el1, x0 // also load PGD to the upper translation based register.
    mrs x2, sctlr_el1
    orr x2, x2, 1
    msr sctlr_el1, x2

_clear_bss:
    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
    //bl  memzero
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

tran:
    ldr x2, =_shell// indirect branch to the virtual address
    br x2

_shell:
    // jump to relocated C code, should not return
4:  bl      main
    // for failsafe, halt this core too
    b       _buzy_loop
