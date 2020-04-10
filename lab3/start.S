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

    // set up EL1
    mrs     x0, CurrentEL
    and     x0, x0, #12 // clear reserved bits

    // running at EL3? 12=1100
    cmp     x0, #12
    bne     5f
    // should never be executed, just for completeness
    mov     x2, #0x5b1
    msr     scr_el3, x2
    mov     x2, #0x3c9
    msr     spsr_el3, x2
    adr     x2, 5f
    msr     elr_el3, x2
    eret

    // running at EL1? 4=0100
5:  cmp     x0, #4
    beq     5f
    msr     sp_el1, x1          // set sp_el1(el1 stack pointer to 0x80000)

    /* 
        enable CNTP for EL1
        aarch64: cnthctl_el2[31:0] mapped to aarch32 cnthctl[31:0]

        cnthctl[1] = cnthctl.PL1PCEN   1 -> No traps Non-secure EL0 and EL1 accesses to the "physical timer registers" to Hyp mode.
        cnthctl[1] = cnthctl.PL1PCTEN  1 -> No traps Non-secure EL0 and EL1 accesses to the "physical counter register" to Hyp mode.
    */
    ; mrs     x0, cnthctl_el2     
    ; orr     x0, x0, #3          
    ; msr     cnthctl_el2, x0     
    ; msr     cntvoff_el2, xzr    //virtual offset zero

    /*
        enable AArch64 in EL1
        set hcr_el2[31] hcr_el2[1] to 1 
        aarch64: [63:32] map to [31:0] so  hcr_el2[31] = hcr_el2[1]
        
        hcr_el2[1] = hcr_el2.SWIO 0b1 -> data cache invalidate
    */
    ; mov     x0, #(1 << 31)      // AArch64
    ; orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    ; msr     hcr_el2, x0
    ; mrs     x0, hcr_el2

    /*
        Setup SCTLR access
        set sctlr_el1 30d00800

        sctlr_el1[11] = sctlr_el.Bit11   RES1.
        sctlr_el1[20] = sctlr_el.UWXN    Any region that is writable at PL0 forced to XN for accesses from software executing at PL1.
        sctlr_el1[22] = sctlr_el.Bit22   RES1.
        sctlr_el1[23] = sctlr_el.SPAN    RES1.
        sctlr_el1[28] = sctlr_el.TRE     TRX remap enable.
        sctlr_el1[29] = sctlr_el.AFE     Access Flag enable
    */
    ; mov     x2, #0x0800
    ; movk    x2, #0x30d0, lsl #16 // movk (Move 16-bit immediate into register, keeping other bits unchanged.) x2 is 0x30d00800
    ; msr     sctlr_el1, x2

    // set up exception handlers for El1
    ldr     x2, =_vectors
    msr     vbar_el1, x2

    /*
        change execution level to EL1
        set spsr_el2 to 0x03c4

        spsr_el2[3:0] = spsr_el2.M   0100 -> stack pointer set to EL1t
        spsr_el2[6] = spsr_el2.F     FIQ interrupt mask      Set to the value of PSTATE.F on taking an exception to EL2, and copied to PSTATE.F on executing an exception return operation in EL2.
        spsr_el2[7] = spsr_el2.I     IRQ interrupt mask      Set to the value of PSTATE.I on taking an exception to EL2, and copied to PSTATE.I on executing an exception return operation in EL2.
        spsr_el2[8] = spsr_el2.A     SError interrupt mask.  Set to the value of PSTATE.A on taking an exception to EL2, and copied to PSTATE.A on executing an exception return operation in EL2.
        spsr_el2[9] = spsr_el2.D     Debug exception mask.   Set to the value of PSTATE.D on taking an exception to EL2, and copied to PSTATE.D on executing an exception return operation in EL2.
    */
    mov     x2, #0x3c4
    msr     spsr_el2, x2

    /*
        set el2 return address to 5f
    */
    adr     x2, 5f
    msr     elr_el2, x2
    eret

5:  mov     sp, x1

    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

    // jump to C code, should not return
4:  bl      main
    // for failsafe, halt this core too
    b       1b

    // important, code has to be properly aligned
    .align 11
_vectors:
    // synchronous
    .align  7
    mov     x0, #0
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    mrs     x3, spsr_el1
    mrs     x4, far_el1
    b       exc_handler

    // IRQ
    .align  7
    mov     x0, #1
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    mrs     x3, spsr_el1
    mrs     x4, far_el1
    b       exc_handler

    // FIQ
    .align  7
    mov     x0, #2
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    mrs     x3, spsr_el1
    mrs     x4, far_el1
    b       exc_handler

    // SError
    .align  7
    mov     x0, #3
    mrs     x1, esr_el1
    mrs     x2, elr_el1
    mrs     x3, spsr_el1
    mrs     x4, far_el1
    b       exc_handler
