_start:
      mrs x0, mpidr_el1
      and x0, x0, 3
      cbz x0, 2f
1:
      wfe
      and x1, x1, 3
      b 1b

2:
      ldr x0, = _start
      mov sp, x0
      bl main
1:
      b 1b

main:
    wfe
    b _start
