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
	  printf ("task id: %d, sp: 0x%p cnt: %d\r\n", (int) get_task_id (),
		  &cnt, cnt++);
	  delay (0.1);
	}
      exit (0);			// all childs exit
    }
}

static void
segfault ()
{
  if (fork () == 0)
    {
      int *a = (int *) 0xdeadbeef;	// a non-mapped address.
      printf ("%d\n", *a);	// trigger simple page fault, child will die here.
    }
}

static void
show_page ()
{
  int free, alloc;
  page_status (&free, &alloc);
  printf ("free: %d\r\nalloc: %d\r\n", free, alloc);
}

static void
mmap_read ()
{
  if (fork () == 0)
    {
      int *ptr = mmap (NULL, 4096, PROT_READ, MAP_ANONYMOUS, (void *) -1, 0);
      printf ("addr: %p\n", ptr);
      printf ("%d\n", ptr[1000]);	// should be 0
      printf ("%d\n", ptr[4097]);	// should be seg fault
    }
}

static void
mmap_write ()
{
  if (fork () == 0)
    {
      int *ptr =
	mmap (NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, (void *) -1,
	      0);
      printf ("addr: %p\n", ptr);
      ptr[1000] = 100;
      printf ("%d\n", ptr[1000]);	// should be 100
      ptr[4097] = 100;		// should be seg fault
      printf ("%d\n", ptr[4097]);	// not reached
    }
}

static void
wrong_permission ()
{
  if (fork () == 0)
    {
      int *ptr = mmap (NULL, 4096, PROT_READ, MAP_ANONYMOUS, (void *) -1, 0);
      printf ("addr: %p\n", ptr);
      printf ("%d\n", ptr[1000]);	// should be 0
      for (int i = 0; i < 4096; ++i)
	{
	  ptr[i] = i + 1;	// should be seg fault
	}
      for (int i = 0; i < 4096; ++i)
	{			// not reached
	  printf ("%d\n", ptr[i]);
	}
    }
}

static void
mmaps ()
{				// test multiple mmaps
  if (fork () == 0)
    {
      for (int i = 0; i < 40; ++i)
	{
	  if (i < 20)
	    {
	      mmap (NULL, 4096, PROT_WRITE | PROT_READ, MAP_ANONYMOUS,
		    (void *) -1, 0);
	    }
	  else if (i < 30)
	    {
	      mmap (NULL, 4096, PROT_WRITE, MAP_ANONYMOUS, (void *) -1, 0);
	    }
	  else
	    {
	      mmap (NULL, 4096, PROT_WRITE | PROT_READ, MAP_ANONYMOUS,
		    (void *) -1, 0);
	    }
	}
      while (1);		// hang to let shell see the mapped regions
    }
}

static void
mmap_unalign ()
{
  if (fork () == 0)
    {
      printf ("0x%p", mmap ((void *) 0x12345678, 0x1fff, PROT_WRITE | PROT_READ, MAP_ANONYMOUS, (void *) -1, 0));	// should be a page aligned address A and region should be A - A +0x2000
      while (1);		// hang to let shell see the mapped regions
    }
}

#pragma GCC optimize("O0")
static int
stack_overflow (int i)
{
  int a[1024] = { 0 };
  printf ("Recursive %d\n", i);
  stack_overflow (i + 1);
  return a[1023] + i;
}

static void
test_stack_overflow ()
{
  if (fork () == 0)
    {
      stack_overflow (0);
    }
}

int
main ()
{
  char buf[BUF_SIZE];
  struct cmd_struct cmd_array[] = {
    {.name = "fork",.description = "test fork",.func = cmd_fork},
    {.name = "page",.description = "page status",.func = show_page},
    {.name = "segfault",.description = "trigger segmentation fault",.func =
     segfault},
    {.name = "mmap_read",.description = "read beyond boundary",.func =
     mmap_read},
    {.name = "mmap_write",.description = "write beyond boundary",.func =
     mmap_write},
    {.name = "mmap_perm",.description = "mmap wrong permission",.func =
     wrong_permission},
    {.name = "mmaps",.description = "multiple mmaps",.func = mmaps},
    {.name = "mmap_unalign",.description = "unaligned mmap",.func =
     mmap_unalign},
    {.name = "stack_overflow",.description = "infinite recursive",.func =
     test_stack_overflow},
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
	{
	  printf ("%s: command not found\r\n", buf);
	  for (i = 0; i < cmd_num; ++i)
	    printf ("%s - %s\r\n", cmd_array[i].name,
		    cmd_array[i].description);
	}
    }
  printf ("%s\r\n", "after main");
  return 0;
}
