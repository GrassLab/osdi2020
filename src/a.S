.section ".text.boot"
.global _start


_start:
      mrs x0, mpidr_el1
      and x0, x0, 3
      cbz x0, 2f
1:
      wfe
      b 1b
2:
      ldr x0, =_stack_top
      mov sp, x0


      ldr x0, =__bss_start
      ldr w2, =__bss_size
      bl main
3:
      b 3b
