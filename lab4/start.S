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

; .macro kernel_entry

; .endm

; .macro  kernel_exit

; .endm

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
    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    /* 
        set up exception handlers for El2
    */
    ldr     x2, =__vectors2
    msr     vbar_el2, x2

    /*
        enable interrupt & virtual interrupt to El0 El1
    */
    mrs     x0, hcr_el2
    orr     x0, x0, 0b10000
    msr     hcr_el2, x0

    /*
        exception level switch to El1
    */
    mov     x0, (1 << 31)           // EL1 use aarch64
    msr     hcr_el2, x0
    mov     x0, 0x3c5               // EL1h (SPSel = 1) with interrupt disabled
    msr     spsr_el2, x0
    adr     x0, EL2_to_El1          // load exception return address
    msr     elr_el2, x0
    adr     x0, _start              // init sp for el1 option 1
    msr     sp_el1, x0
    ldr     x2, =__vectors1
    msr     vbar_el1, x2
    eret                           // return to EL1

EL2_to_El1:
    /*
        exception level switch to El0
    
    mov     x0, 0x3c0               // DAIF & El0
    msr     spsr_el1, x0
    adr     x0, rest_initialization // load exception return address
    msr     elr_el1, x0
    adr     x0, _start              // init sp for el1 option 1
    sub     x0, x0, 0x1000
    msr     sp_el0, x0
    bl      enable_EL0_interrupt

    eret
    */
rest_initialization:

    // bl      disable_interrupt
     bl      enable_interrupt

    /*
        clear bss
    */
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
clear_bss:  
    cbz     w2, c_entry
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, clear_bss

    // jump to C code, should not return
c_entry:  
    bl      main
    // for failsafe, halt this core too
    b       busy_loop