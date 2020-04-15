.section ".text.boot"

.global _start
.global gtime_frq
.global gtime_ct
  
 .macro kernel_entry
  sub	sp, sp, #256
  stp	x0, x1, [sp, #16 * 0]
  stp	x2, x3, [sp, #16 * 1]
  stp	x4, x5, [sp, #16 * 2]
  stp	x6, x7, [sp, #16 * 3]
  stp	x8, x9, [sp, #16 * 4]
  stp	x10, x11, [sp, #16 * 5]
  stp	x12, x13, [sp, #16 * 6]
  stp	x14, x15, [sp, #16 * 7]
  stp	x16, x17, [sp, #16 * 8]
  stp	x18, x19, [sp, #16 * 9]
  stp	x20, x21, [sp, #16 * 10]
  stp	x22, x23, [sp, #16 * 11]
  stp	x24, x25, [sp, #16 * 12]
  stp	x26, x27, [sp, #16 * 13]
  stp	x28, x29, [sp, #16 * 14]
  str	x30, [sp, #16 * 15] 
  .endm

  .macro kernel_exit
  ldp	x0, x1, [sp, #16 * 0]
  ldp	x2, x3, [sp, #16 * 1]
  ldp	x4, x5, [sp, #16 * 2]
  ldp	x6, x7, [sp, #16 * 3]
  ldp	x8, x9, [sp, #16 * 4]
  ldp	x10, x11, [sp, #16 * 5]
  ldp	x12, x13, [sp, #16 * 6]
  ldp	x14, x15, [sp, #16 * 7]
  ldp	x16, x17, [sp, #16 * 8]
  ldp	x18, x19, [sp, #16 * 9]
  ldp	x20, x21, [sp, #16 * 10]
  ldp	x22, x23, [sp, #16 * 11]
  ldp	x24, x25, [sp, #16 * 12]
  ldp	x26, x27, [sp, #16 * 13]
  ldp	x28, x29, [sp, #16 * 14]
  ldr	x30, [sp, #16 * 15] 
  add	sp, sp, #256		
  eret
  .endm

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
    // set up EL1
    mrs     x0, CurrentEL
    and     x0, x0, #12 // clear reserved bits

    // running at EL3?
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

    // running at EL2?
5:  cmp     x0, #8
    bne     6f
    // set up exception handlers
    ldr     x2, =_vectors
    msr     vbar_el1, x2
    //setup EL1 stack
    adr     x2,_start
    msr     sp_el1, x2
    //enable interrupt
    // enable AArch64 in EL1
    mov     x2, #(1 << 31)      // AArch64
    msr     hcr_el2, x2
    mrs     x2, hcr_el2
    orr     x2, x2, #(1 << 4)
    msr     hcr_el2, x2
    
    // switch to EL1
    mov x2, 0x5
    msr spsr_el2, x2
    adr x2, 6f
    msr elr_el2, x2
    eret

6:  mrs     x0, CurrentEL
    and     x0, x0, #12 // clear reserved bits
    cmp     x0, #4
    bne     7f
    //set EL0 stack
    adr     x0, _start
    sub     x0, x0, #0x10000
    msr     sp_el0, x0
    //switch to EL0
    mov     x0,0x0
    msr     spsr_el1, x0
    adr     x0, 7f
    msr     elr_el1, x0
    eret
7:  //mov     sp, x1


    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b    
	
4:  bl      main
    b       1b





  .align 11
.global _vectors
_vectors:
  b no_implement
  .align 7
  b irq_EL2
  .align 7
  b no_implement
  .align 7
  b no_implement
  .align 7

  b synchronous_EL1
  .align 7
  b irq_EL2
  .align 7
  b no_implement
  .align 7
  b no_implement
  .align 7

  b synchronous_EL1
  .align 7
  b irq_EL2
  .align 7
  b no_implement
  .align 7
  b no_implement
  .align 7

  b no_implement
  .align 7
  b irq_EL2
  .align 7
  b no_implement
  .align 7
  b no_implement
  .align 7



synchronous_EL1:
	kernel_entry 
	bl	exception_handler
	kernel_exit 

irq_EL2:
	kernel_entry 
	bl	irq_handler
	kernel_exit 

no_implement:
	kernel_entry
	kernel_exit
	
gtime_frq:
	mrs x0, cntfrq_el0
	ret 
gtime_ct:
	mrs x0, cntpct_el0
	ret 
