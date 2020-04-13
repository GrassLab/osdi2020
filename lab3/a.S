/* let core with cpuid != 0 enter busy loop */
.section ".text"

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
  mrs x1, mpidr_el1          // get cpuid
  and x1, x1, 3              // x0 = x0 & 3
  cbz x1, 2f                 // jump if cpuid!=0
  
loop:
  wfe                        // wait for event
  b loop                     // buzy loop

/* set stack pointer and branch to main function. */
2:
  // set stack before our code
  ldr x1, = _start

check_el2:
  
  // check el2
  mrs     x0, CurrentEL
  and     x0, x0, #2 // clear reserved bits
  cmp     x0, #2

setup_exception_table:
  ldr     x0, =exception_table
  msr     vbar_EL2, x0

to_main:
  mov sp, x1

  // clear bss
  ldr     x1, =__bss_start
  ldr     w2, =__bss_size    // number of words to set zero

set_zero:  
  cbz     w2, 4f
  str     xzr, [x1], #8      // if x0[0] = xzr, then x0 += 8. value in x0 is a address of memory
  sub     w2, w2, #1         // w2 -= 1
  cbnz    w2, set_zero       // if w2[0] != 0, then jump to set_zero

4:
  // jump to C code, should not return
  bl main
  
  // for failsafe, halt this core too
  b loop


// exception
// Simple vector table
.align 11 // vector table should be aligned to 0x800
.global exception_table
exception_table:
  b exception_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
