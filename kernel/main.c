#include <uart.h>
#include <lfb.h>
#include <sched.h>
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
echo_and_delay1 ()
{
  size_t t, cnt, freq;
  uart_puts ("echo 1\n");
  sys_get_time (&cnt, &freq);
  t = cnt;
  while (cnt - t < freq / 2) sys_get_time (&cnt, &freq);
  switch_to (current, &task_pool[1]);
}

void
echo_and_delay2 ()
{
  size_t t, cnt, freq;
  uart_puts ("echo 2\n");
  sys_get_time (&cnt, &freq);
  t = cnt;
  while (cnt - t < freq / 2) sys_get_time (&cnt, &freq);
  switch_to (current, &task_pool[0]);
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
  privilege_task_create (&echo_and_delay1);
  privilege_task_create (&echo_and_delay2);
  switch_to (&fake, &task_pool[0]);
  shell_interactive ();
  return 0;
}
