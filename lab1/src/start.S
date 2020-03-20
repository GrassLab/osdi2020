_start:
      mrs x0, mpidr_el1
      and x0, x0, 3
      cbz x0, 2f
1:
      wfe
      b 1b
2:
      ldr x0, = __bss_start
      ldr x1, = __bss_end
      sub x1, x1, x0
init_mem:
      str xzr, [x0]
      adds x0, x0, #8
      subs x1, x1, #8
      bgt init_mem

      ldr x0, = _start
      mov sp, x0
      bl main
1:
      b 1b
