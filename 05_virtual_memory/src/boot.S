#include "arm/sysregs.h"
#include "mm.h"

.section ".text.boot"

.global _start
_start:
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
    cbz     x0, _main
    b       _loop

_main:
    // top level control at el0, el1; sysregs.h:16
    ldr     x0, =SCTLR_VALUE_MMU_DISABLED
    msr     sctlr_el1, x0
    
    // HCR_RW
    ldr     x0, =HCR_VALUE
    msr     hcr_el2, x0
    
    // Holds the saved process state when an exception happened on EL2
    // mask A, I, F and select stack ptr to EL1h
    ldr     x0, =SPSR_VALUE
    msr     spsr_el2, x0
    
    adr     x0, el1_entry
    msr     elr_el2, x0
    
    eret

el1_entry:
    mov     sp, #LOW_MEMORY

    adr     x0, _bss_begin
    adr     x1, _bss_end
    sub     x1, x1, x0
    bl      memzero

    bl create_page_table

    mov     x0, #VA_START
    mov     x1, #LOW_MEMORY
    add     sp, x0, x1

    // set up translation control reg (el1)
    ldr     x0, =TCR_CONFIG_DEFAULT
    msr     tcr_el1, x0

    // setup MAIR
    ldr     x0, =MAIR_CONFIG_DEFAULT
    msr     mair_el1, x0

    mrs     x2, sctlr_el1
    orr     x2, x2, #1
    msr     sctlr_el1, x2       // enable MMU, cache remains disabled
    
    ldr     x0, =start_kernel
    br      x0
    
    b   _loop

create_page_table:
    mov     x29, x30            // why store x30

    // mov x0, 0x100000         // addr for PGD
    ldr     x0, =pg_dir         // addr for PGD
    lsl     x0, x0, #16
    lsr     x0, x0, #16

    add     x1, x0, #PAGE_SIZE  // addr for 1st PUD // 1GB
    add     x2, x1, #PAGE_SIZE  // addr for 1st PMD // 2MB
    add     x3, x2, #PAGE_SIZE  // addr for 2nd PMD // 2MB
    add     x4, x3, #PAGE_SIZE  // addr for 1st PTE // 4KB

    ldr     x10, =PGD_ATTR
    // ldr     x10, =PD_TABLE
    orr     x10, x1, x10        // addr for 1st PUD
    str     x10, [x0]           // store at 1st PGD

    ldr     x10, =PUD_ATTR
    // ldr     x10, =PD_TABLE
    orr     x10, x2, x10        // addr for 1st PMD
    str     x10, [x1]           // store at 1st PUD

    // ldr     x10, =PUD_ATTR
    ldr     x10, =PD_TABLE
    orr     x10, x3, x10        // addr for 2nd PMD
    str     x10, [x1, 8]        // addr for 2nd PUD
    
    mov     x5, x4              // load 1st PTE addr
    mov     x6, xzr             // index
    mov     x7, #1024           // 2 PMD 
fill_pmd:
    ldr     x8, =PMD_ATTR
    // ldr     x8, =PD_TABLE
    orr     x8, x5, x8
    str     x8, [x2, x6, lsl #3]// PMD + index * 8
    add     x6, x6, #1
    add     x5, x5, #PAGE_SIZE
    cmp     x6, x7
    b.ls fill_pmd

    mov     x5, xzr              // start from 0x0 physical addr
    mov     x6, xzr              // index of entry
    mov     x7, #0x3F000000     // until device memory
fill_normal_mem:
    ldr     x8, =NORMAL_MEM_FLAGS
    orr     x8, x5, x8
    str     x8, [x4, x6, lsl #3]// PTE + index * 8
    add     x6, x6, #1
    add     x5, x5, #PAGE_SIZE
    cmp     x5, x7
    b.ls fill_normal_mem

    mov     x7, #0x41000000
fill_device_mem:
    ldr     x8, =DEVICE_MEM_FLAGS
    orr     x8, x5, x8          // start from 0x3F000000
    str     x8, [x4, x6, lsl #3]
    add     x6, x6, #1
    add     x5, x5, #PAGE_SIZE
    cmp     x5, x7
    b.ls fill_device_mem

    msr     ttbr0_el1, x0
    msr     ttbr1_el1, x0

    mov     x30, x29
    ret

_loop:
    b _loop
