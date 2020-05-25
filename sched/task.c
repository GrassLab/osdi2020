#include <list.h>
#include <string.h>
#include <irq.h>
#include <tlb.h>
#include <mmap.h>
#include "sched.h"

struct task_struct *
privilege_task_create (void (*func) ())
{
  int i;

  for (i = 0; i < POOL_SIZE; ++i)
    if (task_pool[i].task_id == 0)
      break;
  if (i == POOL_SIZE)
    return NULL;
  // init context
  bzero (&task_pool[i].ctx, sizeof (task_pool[i].ctx));
  // kernel space
  task_pool[i].kstack = page_alloc_virt (KPGD, 0, STACK_SIZE / PAGE_SIZE,
					 pd_encode_ram (0));
  task_pool[i].ctx.lr = (size_t) func;
  task_pool[i].ctx.sp = (size_t) task_pool[i].kstack + STACK_SIZE;
  // user space
  task_pool[i].task_id = i + 1;
  task_pool[i].signal_map = 0;
  // init user PGD
  task_pool[i].ctx.PGD =
    (size_t) page_alloc_virt (KPGD, 0, 1, pd_encode_ram (0)) & ~KPGD;
  bzero (task_pool[i].va_maps, sizeof (task_pool[i].va_maps));
  // add to runqueue
  list_add_tail (&task_pool[i].list, runqueue);
  return &task_pool[i];
}

size_t
load_binary (size_t bin_addr)
{
#define DEFAULT_VIRT_ADDR 0x400000
#define NULL_PADDING_SIZE 0x120
#define ENTRY_POINT_OFFSET 0xd9c
  // we only have shell program, hard code it
  extern char _binary_bin_shell_size[];
  extern char _binary_bin_shell_start[];
  size_t page_num;
  void *virt_addr;
  if (bin_addr != (size_t) _binary_bin_shell_start)
    return -1;
  page_num = (size_t) _binary_bin_shell_size + NULL_PADDING_SIZE;
  page_num =
    (page_num % PAGE_SIZE) ? 1 + page_num / PAGE_SIZE : page_num / PAGE_SIZE;
  virt_addr =
    do_mmap ((void *) DEFAULT_VIRT_ADDR, page_num * PAGE_SIZE,
	     PROT_READ | PROT_WRITE | PROT_EXEC, 0, 0, 0);
  if (virt_addr == (void *) -1)
    return -1;
  memcpy (virt_addr + NULL_PADDING_SIZE, _binary_bin_shell_start,
	  (size_t) _binary_bin_shell_size);
  return (size_t) virt_addr + ENTRY_POINT_OFFSET;
}

void
va_map_add (size_t start, size_t size)
{
  struct task_struct *cur = current;
  int i;
  // TODO: lock
  for (i = 0; i < VA_MAP_SIZE; ++i)
    {
      if (cur->va_maps[i].size == 0)
	{
	  cur->va_maps[i].start = start;
	  cur->va_maps[i].size = size;
	  return;
	}
    }
  // TODO: panic
}

void
va_map_clear ()
{
  struct task_struct *cur = current;
  struct va_map_struct *vmap;
  int i;
  for (i = 0; i < VA_MAP_SIZE; ++i)
    {
      vmap = &cur->va_maps[i];
      if (vmap->size != 0)
	{
	  page_free_virt (cur->ctx.PGD | KPGD, vmap->start,
			  vmap->size / PAGE_SIZE);
	  vmap->size = 0;
	  vmap->start = 0;
	}
    }
  cur->stack = 0;
}

int
do_exec (void (*func) ())
{
  size_t entry_point;

  va_map_clear ();
  entry_point = load_binary ((size_t) func);
  if (!current->stack)
    {
      current->stack =
	page_alloc_virt (current->ctx.PGD | KPGD, USER_STACK_ADDR,
			 STACK_SIZE / PAGE_SIZE, pd_encode_ram (0) | PD_RW);
      if (!current->stack)
	return -1;
      va_map_add ((size_t) current->stack, STACK_SIZE);
    }
  asm volatile ("mov x0, %0\n" "mov sp, %1\n"
		"msr sp_el0, x0\n" "msr spsr_el1, xzr\n" "msr elr_el1, %2\n"
		// prevent kernel address leakage
		"mov x0, xzr\n" "mov x1, xzr\n" "mov x2, xzr\n"
		"mov x3, xzr\n" "mov x4, xzr\n" "mov x5, xzr\n"
		"mov x6, xzr\n" "mov x7, xzr\n" "mov x8, xzr\n"
		"mov x9, xzr\n" "mov x10, xzr\n" "mov x11, xzr\n"
		"mov x12, xzr\n" "mov x13, xzr\n" "mov x14, xzr\n"
		"mov x15, xzr\n" "mov x16, xzr\n" "mov x17, xzr\n"
		"mov x18, xzr\n" "mov x19, xzr\n" "mov x20, xzr\n"
		"mov x21, xzr\n" "mov x22, xzr\n" "mov x23, xzr\n"
		"mov x24, xzr\n" "mov x25, xzr\n" "mov x26, xzr\n"
		"mov x27, xzr\n" "mov x28, xzr\n" "mov x29, xzr\n"
		"mov x30, xzr\n" "eret\n"::"r" (current->stack + STACK_SIZE),
		"r" (current->kstack + STACK_SIZE), "r" (entry_point):"x0");
  return 0;
}

int
sys_exec (void (*func) ())
{
  return do_exec (func);
}

size_t
do_get_task_id ()
{
  return current->task_id;
}

size_t
sys_get_task_id ()
{
  return do_get_task_id ();
}

/* syscall only, not in IRQ */
struct trapframe *
get_syscall_trapframe (struct task_struct *task)
{
  struct trapframe *tf;
  tf = (struct trapframe *) (task->kstack + STACK_SIZE - sizeof (*tf));
  return tf;
}

void
va_map_cpy (struct task_struct *new)
{
  struct task_struct *cur = current;
  struct va_map_struct *src, *dst;
  int i;
  void *addr;
  for (i = 0; i < VA_MAP_SIZE; ++i)
    {
      src = &cur->va_maps[i];
      dst = &new->va_maps[i];
      if (src->size != 0)
	{
	  // allocate for temporary use
	  critical_entry ();
	  addr =
	    page_alloc_virt (KPGD, 0, src->size / PAGE_SIZE,
			     pd_encode_ram (0));
	  critical_exit ();
	  // TODO: handle alloc fail
	  if (!addr)
	    printf ("%s\r\n", "TODO: handle alloc fail");
	  // copy it
	  memcpy (addr, (void *) src->start, src->size);
	  // remap to dst's memory
	  unmap_virt (KPGD, (size_t) addr, src->size);
	  if (map_virt_to_phys (new->ctx.PGD | KPGD, src->start,
				(size_t) addr & ~KPGD, src->size,
				pd_encode_ram (0) | PD_RW))
	    {
	      // TODO: handle alloc fail
	      printf ("%s\r\n", "TODO: handle map fail");
	    }
	  *dst = *src;
	}
    }
}

int
do_fork ()
{
  extern void do_fork_child ();
  struct task_struct *new;
  struct trapframe *tf;

  // create a task not in runqueue
  critical_entry ();
  new = privilege_task_create (do_fork_child);
  list_del (&new->list);
  critical_exit ();
  // check allocated task
  if (!new)
    return -1;
  // set kernel stack
  tf = get_syscall_trapframe (new);
  new->ctx.sp = (size_t) tf;
  // copy trapframe and set child return value
  memcpy (tf, get_syscall_trapframe (current), sizeof (struct trapframe));
  tf->x0 = 0;
  // copy user memory
  va_map_cpy (new);

  // add new task to runqeue tail
  critical_entry ();
  list_add_tail (&new->list, runqueue);
  critical_exit ();

  return new->task_id;
}

int
sys_fork ()
{
  return do_fork ();
}

void
page_table_free (size_t *table, int depth)
{
  int i;
  for (i = 0; i < PAGE_SIZE / 8; ++i)
    {
      if (table[i])
	{
	  if (depth >= 3)
	    {
	      // TODO: handle impossible
	      printf ("%s\r\n", "impossible: page_table_free");
	    }
	  else
	    {
	      page_table_free (PD_DECODE (table[i]), depth + 1);
	      page_free_virt (KPGD, (size_t) PD_DECODE (table[i]), 1);
	      table[i] = 0;
	    }
	}
    }
}

void
do_exit (int status)
{
  struct task_struct *cur = current;
  critical_entry ();
  va_map_clear ();
  page_table_free ((size_t *) (cur->ctx.PGD | KPGD), 0);
  page_free_virt (KPGD, cur->ctx.PGD | KPGD, 1);
  cur->exit_status = status;
  list_del (&cur->list);
  list_add_tail (&cur->list, zombiequeue);
  critical_exit ();
  switch_to (current, get_next_task ());
}

void
sys_exit (int status)
{
  do_exit (status);
}
