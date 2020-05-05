#define SYS_GET_TASKID 4

.global get_taskid
get_taskid:
  svc SYS_GET_TASKID
  ret