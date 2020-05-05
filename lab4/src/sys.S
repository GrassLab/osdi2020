#define GET_TASKID 4
#define UART_READ 5
#define UART_WRITE 6
#define EXEC 7

.global get_taskid
get_taskid:
  svc GET_TASKID
  ret

.global exec
exec:
  mov x2, x0
  svc EXEC
  ret