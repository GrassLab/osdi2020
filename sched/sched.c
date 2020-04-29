#include <sched.h>
#include <list.h>

static LIST_HEAD (_runqueue);
struct list_head *runqueue = &_runqueue;

void
schedule ()
{
  struct task_struct *pos = current;
  list_move_tail (&current->list, runqueue);
  switch_to (current, list_entry (runqueue->next, struct task_struct, list));
}
