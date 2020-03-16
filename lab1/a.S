/* let core with cpuid != 0 enter busy loop */
.section ".text"
_start:
  mrs x0, mpidr_el1
  and x0, x0, 3
  cbz x0, 2f
  
1:
  wfe
  b 1b

/* set stack pointer and branch to main function. */
2:
  ldr x0, = _start           // set stack before our code
  mov sp, x0
  bl main                    // jump to C code, should not return
  
  b 1b                       // for failsafe, halt this core too