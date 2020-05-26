#include "sched.h"

int
do_kill (size_t pid, int signal)
{
  struct task_struct *victim;

  // invalid pid
  if (pid > TASK_POOL_LEN || pid <= 1)
    return -1;
  victim = &task_pool[pid - 1];
  // process not exist
  if (victim->task_id == 0)
    return -1;
  // invalid signal
  if (signal < 0 || signal > SIGNAL_MAX)
    return -1;
  victim->signal_map |= 1 << signal;
  return 0;
}

int
sys_kill (size_t pid, int signal)
{
  return do_kill (pid, signal);
}
