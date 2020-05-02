.section ".text"
.global syscall_tables
.global end_syscall_tables
syscall_tables:
  .quad sys_core_timer_enable
  .quad sys_get_time
  .quad sys_uart_read
end_syscall_tables:
