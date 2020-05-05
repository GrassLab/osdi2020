.section ".text"
.global syscall_tables
.global end_syscall_tables
syscall_tables:
  .quad sys_core_timer_enable
  .quad sys_get_time
  .quad sys_uart_read
  .quad sys_uart_write
  .quad sys_exec
  .quad sys_get_task_id		  /* 5 */
  .quad sys_fork
  .quad sys_exit
  .quad sys_kill
end_syscall_tables:
