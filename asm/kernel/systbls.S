.section ".text"
.global syscall_tables
.global end_syscall_tables
syscall_tables:
  .align 4
  .quad sys_core_timer_enable	  /* 0 */
  .align 4
  .quad sys_get_time
  .align 4
  .quad sys_uart_read
  .align 4
  .quad sys_uart_write
  .align 4
  .quad sys_exec
  .align 4
  .quad sys_get_task_id		  /* 5 */
  .align 4
  .quad sys_fork
  .align 4
  .quad sys_exit
  .align 4
  .quad sys_kill
  .align 4
  .quad sys_page_status
end_syscall_tables:
