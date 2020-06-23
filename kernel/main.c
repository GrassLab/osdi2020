#include <uart.h>
#include <lfb.h>
#include <sched.h>
#include <string.h>
#include <timer.h>
#include <syscall.h>
#include <vfs.h>
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
idle ()
{
  while (1)
    {
      zombie_reaper ();
      delay (1);
    }
}

static void
user_shell ()
{
  extern void _binary_bin_shell_start ();
  do_exec (_binary_bin_shell_start);
}

static void
fat32_test ()
{
  struct file *a = vfs_open ("TEST", 0);
  char buf[5];
  buf[4] = '\0';
  a->f_pos = 511;
  vfs_read (a, buf, 4);
  printf ("pos: %d, value: %s\r\n", (int) a->f_pos, buf);
  a->f_pos = 511;
  printf ("modify to: ");
  do_uart_read ((char *) buf, 4);
  printf ("%s\r\n", buf);
  vfs_write (a, buf, 4);
  a->f_pos = 511;
  vfs_read (a, buf, 4);
  printf ("pos: %d, value: %s\r\n", (int) a->f_pos, buf);
}

int
main (int error, char *argv[])
{
  struct task_struct fake;
  // init stack guard. It should be random, but I'm lazy.
  __stack_chk_guard = (void *) 0xdeadbeef;

  uart_init ();
  lfb_init ();
  uart_puts (BOOT_MSG);
  if (error)
    {
      uart_puts ("---------- Warning ----------\n");
      uart_puts (argv[0]);
      uart_puts ("-----------------------------\n");
    }

  rootfs_init ();
  fat32_test ();
  task_init ();
  privilege_task_create (idle);
  privilege_task_create (user_shell);
  sys_core_timer_enable ();
  switch_to (&fake, &task_pool[0]);
  shell_interactive ();
  return 0;
}
