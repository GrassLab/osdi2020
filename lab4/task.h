void self_cpntext_switch();
void privilege_task_create(void (*do_func)());
void schedule();
#ifndef _COMMON
#define _COMMON
typedef struct t_task {
  int id;
  void (*func)();
} task;
#endif
task task_pool[64];
int runqueue[64];