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


clear_bss:
    # clear bss
    ldr     x1, =__bss_start
    ldr     x2, =__bss_size
    
cmp_addr: 
    cbz     x2, stack_init
    str     xzr, [x1], #8
    sub     x2, x2, #1
    cbnz    x2, cmp_addr
    
stack_init:  
    # set stack before our code
    ldr     x0, =_start
    mov     sp, x0

from_el2_to_el1:

    # EL1 use aarch64
    mov x1, #(1 << 31) 
    msr HCR_EL2, x1

    # EL1h (SPSel = 1) with interrupt disabled
    # SPSR_EL2: Holds the saved process state when an exception is taken to EL2.
    # M[3:0]: AArch64 Exception level and selected Stack Pointer. 0b0101 -> EL1h
    # FIAD[9:6]
    mov     x1, 0b001111000101
    msr     SPSR_EL2, x1

    # set up exception handler for el2
    ldr     x1, =exception_table_el2
    msr     VBAR_EL2, x1
    
    # set up exception handler for el1
    ldr     x1, =exception_table_el1
    msr     VBAR_EL1, x1

    # init sp for EL1
    sub x0, x0, #0x10000
    msr SP_EL1, x0

    # load exception return address
    adr x1, return_to_el1
    msr ELR_EL2, x1

    # return to EL1
    eret 

return_to_el1:

    # disable exception for accessing SIMD reg in EL1
    # CPACR_EL1: Controls access to trace, SVE, Advanced SIMD and floating-point functionality.
    # FPEN, bits[21:20]: Traps EL0 and EL1 accesses to the SVE, Advanced SIMD, and floating-point registers to EL1, or to EL2 when it is implemented and enabled for the current Security state and HCR_EL2.TGE is 1, from both Execution states.
    mrs x1, CPACR_EL1
    orr x1, x1, (3 << 20) // # #0b1100110000000000000000
    # orr x1, x1, (1 << 16) 
    msr CPACR_EL1, x1

    # disable interrupt in el1
    mov     x1, 0x3C0
    msr     SPSR_EL1, x1

    # init sp for EL0
    sub x0, x0, #0x10000
    msr SP_EL0, x0

    # lab4. start from el1
    bl      el1_main
    b       loop
    
    # load exception return address
    adr x1, return_to_el0
    msr ELR_EL1, x1

    # return to EL0
    eret

return_to_el0: 
    
jmp_main:  
    
    # jump to C code, should not return
    bl      main
    # for failsafe, halt this core too
    b       loop

loop:  
    wfe
    b       loop