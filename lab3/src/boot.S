#include "mm.h"
#include "sysregs.h"

.section ".text.boot"

.global _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, 3
    cbz x0, continue

proc_hang:
    wfe
    b proc_hang

continue:

#ifndef RUN_ON_EL2
    ldr	x0, =SCTLR_VALUE_MMU_DISABLED
    msr	sctlr_el1, x0		

    ldr	x0, =HCR_VALUE
    msr	hcr_el2, x0

    ldr	x0, =SPSR_VALUE
    msr	spsr_el2, x0

    adr	x0, el1_entry		
    msr	elr_el2, x0

    eret
#endif

el1_entry:

#ifdef RUN_ON_EL2
    // load exception_table
    ldr x0, =exception_table
    msr vbar_el2, x0
#else
    ldr x0, =exception_table
    msr vbar_el1, x0
#endif
    
    // init bss
    adr x0, __bss_beg
    adr x1, __bss_end
    bl  memzero

    // set stack
    ldr x0, =__stack_top
    mov sp, x0 

    // jump to main
    bl main

init_irq:

    ldr x0, =HCR_RW
    msr hcr_el2, x0
    msr daifclr, #2
    ret

// Simple vector table
.align 11 // vector table should be aligned to 0x800
.global exception_table
exception_table:
  bl exception_handler // 0x0
  eret
  .align 7
  bl exception_handler // 0x80
  eret
  .align 7
  bl exception_handler // 0x100
  eret
  .align 7
  bl exception_handler // 0x180
  eret
  .align 7

  bl synchronous_exceptions // 0x200
  eret
  .align 7
  bl irq_handler            //0x280
  eret
  .align 7
  bl exception_handler      //0x300
  eret
  .align 7
  bl exception_handler      //0x380
  eret
  .align 7

  bl exception_handler //0x400
  eret
  .align 7
  bl exception_handler //0x480
  eret
  .align 7
  bl exception_handler //0x500
  eret
  .align 7
  bl exception_handler //0x580
  eret
  .align 7

  bl exception_handler //0x600
  eret
  .align 7
  bl exception_handler //0x680
  eret
  .align 7
  bl exception_handler //0x700
  eret
  .align 7
  bl exception_handler //0x780
  eret
  .align 7
