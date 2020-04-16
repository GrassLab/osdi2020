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
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    // cpu id > 0, stop
1:  wfe
    b       1b
2:  // cpu id == 0

    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    // --------------------debug--------------------- //
    mrs     x0, CurrentEL
    lsr     x0, x0, #2
    // enable AArch64 in EL1
    mov     x2, #(1 << 31)      // AArch64
    msr     hcr_el2, x2
    mrs     x2, hcr_el2

    // set EL1 stack
    msr     sp_el1, x1
    
	// change execution level to EL1
    mov     x2, #0x5     // EL1h, using SP_EL1
    msr     spsr_el2, x2
    adr     x2, 5f
    msr     elr_el2, x2
    eret

    // set exception vector table
5:  ldr     x2, =vector_table
    msr     VBAR_EL1, x2

    // --------------------debug-------------------- //
    mrs     x0, CurrentEL
    lsr     x0, x0, #2
    // disable exception of access SIMD register in EL1
    mrs     x2, CPACR_EL1
    orr     x2, x2, #(3 << 20)
    msr     CPACR_EL1, x2

    // set EL0 stack
    sub     x1, x1, #0x10000
    msr     sp_el0, x1

    // change execution level to EL0
    mov     x2, #0      // EL0t, using SP_EL0
    msr     spsr_el1, x2
    adr     x2, 5f
    msr     elr_el1, x2
    eret
5:


    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

4: 
    // jump to C code, should not return
    bl      main
    // for failsafe, halt this core too
    b       1b

