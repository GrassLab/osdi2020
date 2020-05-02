.section ".text"
.global syscall_tables
.global end_syscall_tables
syscall_tables:
  .align 3
  .quad sys_core_timer_enable
  .align 3
  .quad sys_get_time
end_syscall_tables:
