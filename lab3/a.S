/* let core with cpuid != 0 enter busy loop */
.section ".text"

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