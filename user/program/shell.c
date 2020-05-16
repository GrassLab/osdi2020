#include <stdlib.h>
#define BUF_SIZE 0x30

struct cmd_struct
{
  char *name;
  char *description;
  void (*func) ();
};

int
read_command (char *buf, size_t len)
{
  char c;
  size_t i;
  for (i = 0; i < len - 1; ++i)
    {
      uart_read (&c, 1);
      uart_write (&c, 1);
      if (c == '\r')
	{
	  c = '\n';
	  uart_write (&c, 1);
	  break;
	}
      buf[i] = c;
    }
  buf[i] = '\0';
}

static void
cmd_fork ()
{
  int cnt = 0;
  if (fork () == 0)
    {
      fork ();
      fork ();
      while (cnt < 10)
	{
	  // address should be the same across tasks, but the cnt should be increased indepndently
	  printf ("task id: %d, sp: 0x%p cnt: %d\n", (int) get_task_id (), &cnt, cnt++);
	  delay (0.1);
	}
      exit (0);			// all childs exit
    }
}

static void
segfault ()
{
  if(fork() == 0)
    {
      int* a = 0xdeadbeef; // a non-mapped address.
      printf("%d\n", *a); // trigger simple page fault, child will die here.
    }
}

static void
show_page ()
{
  int free, alloc;
  page_status (&free, &alloc);
  printf ("free: %d\r\nalloc: %d\r\n", free, alloc);
}

int
main ()
{
  char buf[BUF_SIZE];
  struct cmd_struct cmd_array[] = {
    {.name = "fork",.description = "test fork",.func = cmd_fork},
    {.name = "page",.description = "page status",.func = show_page},
    {.name = "segfault",.description = "trigger segmentation fault",.func = segfault}
  };
  int i;
  int cmd_num = sizeof (cmd_array) / sizeof (struct cmd_struct);
  while (1)
    {
      uart_write ("# ", 2);
      read_command (buf, BUF_SIZE);
      for (i = 0; i < cmd_num; ++i)
	{
	  if (!strncmp (buf, cmd_array[i].name, strlen (cmd_array[i].name)))
	    {
	      cmd_array[i].func ();
	      break;
	    }
	}
      if (i >= cmd_num)
	printf ("%s: command not found\r\n", buf);
    }
  printf ("%s\r\n", "after main");
  return 0;
}
