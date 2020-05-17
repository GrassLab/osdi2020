#include <sched.h>
#include <list.h>
#include <irq.h>
#include <tlb.h>

static LIST_HEAD (_runqueue);
struct list_head *runqueue = &_runqueue;

static LIST_HEAD (_zombiequeue);
struct list_head *zombiequeue = &_zombiequeue;

struct task_struct *
get_next_task ()
{
  struct task_struct *next;
  next = list_entry (runqueue->next, struct task_struct, list);
  return next;
}

void
schedule ()
{
  struct task_struct *pos = current;

  if (!pos->resched)
    return;
  if (current->signal_map & (1 << SIGKILL))
    {
      do_exit (0);
    }
  else
    {
      // move self to tail
      pos->resched = 0;
      list_move_tail (&current->list, runqueue);
      switch_to (current, get_next_task ());
    }
}

void
zombie_reaper ()
{
  struct task_struct *victim;
  if (list_empty (zombiequeue))
    return;
  victim = list_entry (zombiequeue->next, struct task_struct, list);
  list_del (&victim->list);
  page_free_virt (KPGD, (size_t) victim->kstack, STACK_SIZE / PAGE_SIZE);
  victim->kstack = 0;
  victim->task_id = 0;
}
