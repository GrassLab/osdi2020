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
    ldr     x2, =_vectors
    msr     vbar_el2, x2

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

    // important, code has to be properly aligned
    .align 11
_vectors:
//Exception from current EL, while using sp_el0
    // synchronous
    b       sync_exc_handler
    .align  7
    // IRQ
    b       sync_exc_handler
    .align  7
    // FIQ
    b       sync_exc_handler
    .align  7
    // SError
    b       sync_exc_handler
    .align  7

//Exception from current EL, while using sp_elx
    // synchronous
    b       sync_exc_handler
    .align  7
    // IRQ
    b       irq_exc_handler
    .align  7
    // FIQ
    b       SError_handler
    .align  7
    // SError
    b       SError_handler
    .align  7
