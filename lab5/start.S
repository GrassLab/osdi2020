/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "sysregs.h"
#include "mmu.h"

.section ".text.boot"

.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, boot
    // cpu id > 0, stop
busy_loop:  
    wfe
    b       busy_loop
    
boot:
    //disable MMU
    ldr     x0, = SCTLR_MMU_DISABLE
    msr     sctlr_el1, x0

    /*
        exception level switch to El1
    */
    ldr     x0, = HCR_VALUE           
    msr     hcr_el2, x0

    ldr     x0, = SPSR_EL2_VAL              
    msr     spsr_el2, x0

    adr     x0, EL2_to_El1          // load exception return address
    msr     elr_el2, x0

    adr     x0, _start              // init sp for el1 option 1
    msr     sp_el1, x0

    //ldr     x2, =__vectors1
    //msr     vbar_el1, x2

    eret                           // return to EL1

EL2_to_El1:
    ldr     x0, = SPSR_EL1_VAL
    msr     spsr_el1, x0
    //bl      enable_EL0_interrupt

    

    /*
        clear bss
    */
    adr     x1, __bss_start
    adr     x2, __bss_end
    sub     x2, x2, x1
clear_bss:  
    cbz     x2, c_entry
    str     xzr, [x1], #8
    sub     x2, x2, #1
    cbnz    x2, clear_bss

    bl      mmu_init

    ldr     x0, =_start
    mov     sp, x0

    ldr     x1, = main

    ldr     x0, = SCTLR_MMU_ENABLE
    msr     sctlr_el1, x0 
    
    br      x1
    

    // jump to C code, should not return
c_entry:  
    bl      main
    // for failsafe, halt this core too
    b       busy_loop

