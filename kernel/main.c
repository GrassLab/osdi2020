#include <uart.h>
#include <lfb.h>
#include <sched.h>
#include <string.h>
#include <timer.h>
#include <syscall.h>
#include "shell.h"
#include "irq.h"

#define BOOT_MSG "                                                               \n" \
                 "                                                               \n" \
                 "    )                                                      ____\n" \
                 " ( /(       (   (            (  (               (   (     |   /\n" \
                 " )\\())   (  )\\  )\\           )\\))(   '     (    )\\  )\\ )  |  /\n" \
                 "((_)\\   ))\\((_)((_) (       ((_)()\\ )  (   )(  ((_)(()/(  | /\n" \
                 " _((_) /((_)_   _   )\\      _(())\\_)() )\\ (()\\  _   ((_)) |/\n" \
                 "| || |(_)) | | | | ((_)  )  \\ \\((_)/ /((_) ((_)| |  _| | (\n" \
                 "| __ |/ -_)| | | |/ _ \\ /(   \\ \\/\\/ // _ \\| '_|| |/ _` | )\\\n" \
                 "|_||_|\\___||_| |_|\\___/(_))   \\_/\\_/ \\___/|_|  |_|\\__,_|((_)\n" \
                 "                                                                  \n"

void
echo_and_delay ()
{
  double t;
  char buf[10];
  int c;
  c = uart_read (buf, 10);
  printf ("%d: %s\r\n", c, buf);
  while (1)
    {
      printf ("echo haha %f\r\n", t);
      t = get_time ();
      while (get_time () - t < 1);
    }
}

void
echo_and_do_exec ()
{
  printf ("echo kernel %d\r\n", (int) current->task_id);
  do_exec (&echo_and_delay);
}

int
main (int error, char *argv[])
{
  // init stack guard. It should be random, but I'm lazy.
  __stack_chk_guard = (void *) 0xdeadbeef;

  struct task_struct fake;
  uart_init ();
  lfb_init ();
  uart_puts (BOOT_MSG);
  if (error)
    {
      uart_puts ("---------- Warning ----------\n");
      uart_puts (argv[0]);
      uart_puts ("-----------------------------\n");
    }
  privilege_task_create (&echo_and_do_exec);
  privilege_task_create (&echo_and_do_exec);
  privilege_task_create (&echo_and_do_exec);
  privilege_task_create (&echo_and_do_exec);
  privilege_task_create (&echo_and_do_exec);
  sys_core_timer_enable ();
  switch_to (&fake, &task_pool[0]);
  shell_interactive ();
  return 0;
}
