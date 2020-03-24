#include "include/mm.h"

// very thing defined in boot.S should go in the .text.boot section
.section ".text.boot"

.globl _start
_start:
    mrs    x0, mpidr_el1       // put mpidr_el1(process id) in x0
    and    x0, x0,#0xFF        // Check processor id(x0= x0 and 0xFF)
    cbz    x0, master          // Hang for all non-primary CPU
			       //(if x0 == 0 jump to master)
    b    proc_hang             //Else it will become a for loop

proc_hang: 
    b proc_hang

master:
    adr    x0, bss_begin
    adr    x1, bss_end
    sub    x1, x1, x0
    bl     memzero             //branch to 'memzero' 
			       //(next instr's addr will be copy in to lr reg)

    mov    sp, #LOW_MEMORY     //mov stack pointer to position LOW_MEMORY
    bl    kernel_main
	
    b proc_hang    // for failsafe, halt this core too         
