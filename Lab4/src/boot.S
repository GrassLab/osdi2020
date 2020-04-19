.section ".text.boot"

#include "include/mm.h"

.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x0, mpidr_el1  // put mpidr_el1(process id) in x0
    and     x0, x0, #0xFF // Check processor id(x0= x0 and 0xFF)
    cbz     x0, master // Hang for all non-primary CPU
    
    //(if x0 == 0 jump to master)
    b       proc_hang    //Else it will become a for loop


proc_hang:
    b proc_hang

master:    
    /*** Now return from el2 to el1 ***/ 
    mov x1,#LOW_MEMORY
    msr sp_el1,x1  //set stack pointer
    
    // set HCR_EL2 RW bit to 1 
    // since The execution state of EL1 is AArch64
    mrs x0, hcr_el2
    orr x0, x0, #(1<<31) // RW=1 EL1 Execution state is AArch64.
    msr hcr_el2, x0
    
    // set up exception handlers
    ldr     x2, = exception_table
    msr     vbar_el1, x2

    // change execution level
    mov x2, #0b0101 //selcet stack pointer for EL1h
    msr spsr_el2,x2
    
    adr x2,el1_entry 
    msr elr_el2,x2
    eret

el1_entry: 
    // clear bss
    ldr     x1, =_bss_begin
    ldr     w2, =__bss_size
clean:
    cbz     w2, main
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, clean

    // jump to C code, should not return
main:  
    bl      kernel_main
    // for failsafe, halt this core too
    b       proc_hang


