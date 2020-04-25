.section ".text.boot"

#define SCTLR_RESERVED                  (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN          (0 << 25)
#define SCTLR_I_CACHE_DISABLED          (0 << 12)
#define SCTLR_D_CACHE_DISABLED          (0 << 2)
#define SCTLR_MMU_DISABLED              (0 << 0)


#define SCTLR_VALUE_MMU_DISABLED	(SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)

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
    
    //disable MMU
    ldr	x0, =SCTLR_VALUE_MMU_DISABLED
    msr	sctlr_el1, x0	

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
    //disable exception of access SIMD register in EL1(for printf)
    mrs x0, CPACR_EL1
    orr x0, x0, #(3 << 20)
    msr CPACR_EL1, x0

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


