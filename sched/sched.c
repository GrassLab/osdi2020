#include <sched.h>
#include <list.h>
#include <irq.h>

static LIST_HEAD (_runqueue);
struct list_head *runqueue = &_runqueue;

static LIST_HEAD (_zombiequeue);
struct list_head *zombiequeue = &_zombiequeue;

void
schedule ()
{
  struct task_struct *pos = current;
  struct task_struct *next;

  if (!pos->resched)
    return;
  // move self to tail
  pos->resched = 0;
  list_move_tail (&current->list, runqueue);
  // move idle task (task_pool[0]) to tail
  next = list_entry (runqueue->next, struct task_struct, list);
  if (next == &task_pool[0])
    {
      list_move_tail (&next->list, runqueue);
      next = list_entry (runqueue->next, struct task_struct, list);
    }
  switch_to (current, next);
}

void
zombie_reaper ()
{
  struct task_struct *victim;
  if (list_empty (zombiequeue))
    return;
  disable_irq ();
  victim = list_entry (zombiequeue->next, struct task_struct, list);
  list_del (&victim->list);
  victim->task_id = 0;
  enable_irq ();
}
