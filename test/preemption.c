#include <sched.h>
#include <string.h>
#include <timer.h>

static void
delay (int sec)
{
  size_t t, cnt, freq;
  sys_get_time (&cnt, &freq);
  t = cnt;
  while ((cnt - t) / freq < sec)
    sys_get_time (&cnt, &freq);
}

static void
foo ()
{
  while (1)
    {
      printf ("Task id: %d\n", (int) current->task_id);
      delay (1);
      current->resched = 1;
      schedule ();
    }
}

static void
idle ()
{
  while (1)
    {
      current->resched = 1;
      schedule ();
      delay (1);
    }
}

void
test_preemption ()
{
  struct task_struct fake;
  int N = 5;

  // first privilege for idle
  privilege_task_create (idle);
  for (int i = 0; i < N; ++i)
    {				// N should > 2
      privilege_task_create (foo);
    }
  switch_to (&fake, &task_pool[0]);
}
