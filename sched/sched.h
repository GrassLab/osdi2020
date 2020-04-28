#ifndef SCHED
#define SCHED

#include <stddef.h>
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
};

struct task_struct
{
  struct context ctx;
  size_t task_id;
  char stack[0x1000] __attribute__ ((aligned (8)));
  char kstack[0x1000] __attribute__ ((aligned (8)));
} task_pool[POOL_SIZE];

void privilege_task_create (void (*func) ());

#define current get_current()
extern struct task_struct *get_current ();
extern void switch_to (struct task_struct *cur, struct task_struct *next);


#endif /* ifndef SCHED */
