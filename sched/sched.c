#include <sched.h>
#include <list.h>

static LIST_HEAD (_run_queue);
struct list_head *run_queue = &_run_queue;

void
schedule ()
{
  struct task_struct *pos = current;
  list_move_tail (&current->list, run_queue);
  switch_to (current, list_entry (run_queue->next, struct task_struct, list));
}
