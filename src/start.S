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

    ldr     x0, =HCR_VALUE
    msr     hcr_el2, x0

	//ldr	    x0, =SCR_VALUE
	//msr	    scr_el3, x0
    
    ldr     x0, =SPSR_VALUE
    msr     spsr_el2, x0

    adr     x0, el1_entry
    msr     elr_el2, x0

    // from el2 to el1
    eret

el1_entry:
//clear_bss:
    ldr     w1, =__bss_size
    cbz     w1, pseudo_main
    ldr     x0, =__bss_start
    bl      memzero

    ldr     x0, =EL1_LOW_MEMORY 
    mov     sp, x0

    ldr     x0, =exception_table
    msr     vbar_el1, x0

    ldr     x0, 0
    msr     spsr_el1, x0

    adr     x0, el0_entry
    msr     elr_el1, x0

    // from el1 to el0
    eret

el0_entry:
    ldr     x0, =EL0_LOW_MEMORY 
    mov     sp, x0


pseudo_main:
    bl      main                  // jump to C code, should not return
    b       pe_hang               // for failsafe, halt this core too