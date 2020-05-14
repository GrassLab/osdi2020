#include <sched.h>
#include <string.h>
#include <timer.h>
#include <syscall.h>
#include <irq.h>
#include <uart.h>

static void
delay (size_t sec)
{
  size_t t, cnt, freq;
  sys_get_time (&cnt, &freq);
  t = cnt;
  while ((cnt - t) / freq < sec)
    sys_get_time (&cnt, &freq);
}

static void
user_delay (double sec)
{
  double t;
  t = get_time ();
  while (get_time () - t < sec);
}

static void
test ()
{
  int cnt = 1;
  int pid = fork ();
  if (pid == 0)
    {
      while (1)
	{
	  printf ("kill me %d\r\n", (int) get_task_id ());
	  user_delay (0.5);
	}
    }
  else
    {
      printf ("kill %d after 2 seconds\r\n", pid);
      user_delay (2);
      kill (pid, SIGKILL);
      exit (0);
    }
}

static void
user_test ()
{
  do_exec (test);
}

static int
get_runable_task_num ()
{
  struct list_head *p;
  int cnt;
  disable_irq ();
  cnt = 0;
  list_for_each (p, runqueue)
  {
    cnt++;
  }
  enable_irq ();
  return cnt;
}

static void
idle ()
{
  while (1)
    {
      if (get_runable_task_num () == 1)
	break;
      zombie_reaper ();
      delay (1);
    }
  uart_puts ("test finished");
  while (1);
}

static void
zombie_reaper_loop ()
{
  enable_irq ();
  while (1)
    {
      if (get_runable_task_num () == 2)
	break;
      zombie_reaper ();
      delay (1);
    }
  exit (0);
}

void
test_signal ()
{
  struct task_struct fake;

  // first privilege for idle
  privilege_task_create (idle);

  privilege_task_create (user_test);

  sys_core_timer_enable ();
  switch_to (&fake, &task_pool[0]);
}
