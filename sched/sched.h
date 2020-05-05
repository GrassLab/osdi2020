#ifndef SCHED
#define SCHED

#include <stddef.h>
#include <list.h>
#include <signal.h>

#define POOL_SIZE 64
#define STACK_SIZE 0x1000

struct trapframe
{
  size_t x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
  size_t x10, x11, x12, x13, x14, x15, x16, x17, x18, x19;
  size_t x20, x21, x22, x23, x24, x25, x26, x27, x28, fp;
  size_t sp, sp_el0, elr_el1, spsr_el1;
};

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
};

struct task_struct
{
  struct context ctx;
  size_t task_id;
  char stack[STACK_SIZE] __attribute__ ((aligned (8)));
  char kstack[STACK_SIZE] __attribute__ ((aligned (8)));
  struct list_head list;
  char resched;
  size_t signal_map;
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
struct trapframe *get_syscall_trapframe (struct task_struct *task);
struct task_struct *get_next_task ();
int do_kill (size_t pid, int signal);
int sys_kill (size_t pid, int signal);

#endif /* ifndef SCHED */
