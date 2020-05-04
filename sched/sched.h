#ifndef SCHED
#define SCHED

#include <stddef.h>
#include <list.h>

#define POOL_SIZE 64

struct context
{
  size_t x19;
  size_t x20;
  size_t x21;
  size_t x22;
  size_t x23;
  size_t x24;
  size_t x25;
  size_t x26;
  size_t x27;
  size_t x28;
  size_t fp;
  size_t lr;
  size_t sp;
  size_t trapframe;
};

struct task_struct
{
  struct context ctx;
  size_t task_id;
  char stack[0x1000] __attribute__ ((aligned (8)));
  char kstack[0x1000] __attribute__ ((aligned (8)));
  struct list_head list;
  char resched;
} task_pool[POOL_SIZE];

struct task_struct *privilege_task_create (void (*func) ());

#define current get_current()
extern struct task_struct *get_current ();
extern void switch_to (struct task_struct *cur, struct task_struct *next);
struct list_head *runqueue;
struct list_head *zombiequeue;
void schedule ();
int do_exec (void (*func) ());
int sys_exec (void (*func) ());
size_t do_get_task_id ();
size_t sys_get_task_id ();
int do_fork ();
int sys_fork ();
void sys_exit (int status);
void do_exit (int status);
void zombie_reaper ();

#endif /* ifndef SCHED */
