#include <list.h>
#include <string.h>
#include "sched.h"

void
privilege_task_create (void (*func) ())
{
  int i;

  for (i = 0; i < POOL_SIZE; ++i)
    if (task_pool[i].task_id == 0)
      break;
  bzero (&task_pool[i].ctx, sizeof (task_pool[i].ctx));
  task_pool[i].task_id = i + 1;
  task_pool[i].ctx.lr = (size_t) func;
  task_pool[i].ctx.sp = (size_t) &task_pool[i].kstack[0x1000];
  list_add_tail (&task_pool[i].list, run_queue);
}
