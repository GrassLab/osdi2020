#include "mm.h"
#define HCR_RW           (1 << 31)
#define HCR_VALUE        HCR_RW

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
    // load exception_table to VBAR_EL2
    ldr x0, =exception_table
    msr vbar_el2, x0
    
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
  bl irq_handler
  eret
  .align 7
  bl exception_handler
  eret
  .align 7
  bl exception_handler
  eret
  .align 7

  bl exception_handler
  eret
  .align 7
  bl exception_handler
  eret
  .align 7
  bl exception_handler
  eret
  .align 7
  bl exception_handler
  eret
  .align 7

  bl exception_handler
  eret
  .align 7
  bl exception_handler
  eret
  .align 7
  bl exception_handler
  eret
  .align 7
  bl exception_handler
  eret
  .align 7
