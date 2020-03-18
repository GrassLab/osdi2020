/* let core with cpuid != 0 enter busy loop */
.section ".text"
_start:
  mrs x0, mpidr_el1          // get cpuid
  and x0, x0, 3              // x0 = x0 & 3
  cbz x0, 2f                 // jump if cpuid!=0
  
loop:
  wfe                        // wait for event
  b loop                     // buzy loop

/* set stack pointer and branch to main function. */
2:
  // set stack before our code
  ldr x0, = _start
  mov sp, x0

  // clear bss
  ldr     x0, =__bss_start
  ldr     w2, =__bss_size    // number of words to set zero

set_zero:  
  cbz     w2, 4f
  str     xzr, [x0], #8      // if x0[0] = xzr, then x0 += 8. value in x0 is a address of memory
  sub     w2, w2, #1         // w2 -= 1
  cbnz    w2, set_zero       // if w2[0] != 0, then jump to set_zero

4:
  // jump to C code, should not return
  bl main
  
  // for failsafe, halt this core too
  b loop