#include "sysregs.h"
#include "mm.h"
.section ".text.boot"

.global _start
_start:
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF         // get CPU ID and mask reserved bits
    cbz     x0, master

pe_hang:                          // hang for all non-primary CPU
    wfe                           // enter low power mode
    b       pe_hang

master:
    ldr     x0, =SCTLR_VALUE_MMU_DISABLED
    msr     sctlr_el1, x0

    ldr     x0, =HCR_EL2_VALUE    // set EL1 aarch
    msr     hcr_el2, x0

    ldr     x0, =SPSR_EL2_VALUE   // set processor restore state after calling eret  
    msr     spsr_el2, x0

    adr     x0, el1_entry         // eret return address
    msr     elr_el2, x0

    ldr     x0, =EL1_LOW_MEMORY
    msr     sp_el1, x0

    eret                          // from el2 to el1
// void *memset(void *s, int c, size_t n)
el1_entry:
    ldr     x2, =__bss_size       // clear_bss
    cbz     x2, main_entry
    ldr     x0, =__bss_start
    mov     w1, #0
    bl      memset

    ldr     x0, =exception_table  // set up el1 exception table
    msr     vbar_el1, x0

main_entry:
    mov     sp, #(2 * SECTION_SIZE)
    bl      kernel_main           // jump to C code, should not return
    b       pe_hang               // for failsafe, halt this core too