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

.section ".text.boot"

.global _start

_start:
    # read cpu id, stop slave cores
    mrs     x1, MPIDR_EL1
    and     x1, x1, #3
    
    # cpu id > 0, stop
    cbnz     x1, loop
    

setup_exception_table:
    ldr     x1, =exception_table
    msr     VBAR_EL2, x1

stack_init:  

    # set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    # clear bss
    ldr     x1, =__bss_start
    ldr     x2, =__bss_size
    
cmp_addr: 
    cbz     x2, jmp_main
    str     xzr, [x1], #8
    sub     x2, x2, #1
    cbnz    x2, cmp_addr
    
jmp_main:  
    
    # jump to C code, should not return
    bl      main
    # for failsafe, halt this core too
    b       loop

loop:  
    wfe
    b       loop