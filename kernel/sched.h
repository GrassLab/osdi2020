#ifndef KERNEL_SCHED_H_
#define KERNEL_SCHED_H_

#include "kernel/lib/queue.h"
#include "kernel/lib/types.h"

#define MAX_TASK_NUM 64
#define MAX_STACK_SIZE 4096
#define DEFAULT_TIMESLICE 1

struct task_context {
  /* Callee saved registers */
  uint64_t x19;
  uint64_t x20;
  uint64_t x21;
  uint64_t x22;
  uint64_t x23;
  uint64_t x24;
  uint64_t x25;
  uint64_t x26;
  uint64_t x27;
  uint64_t x28;

  uint64_t fp;
  uint64_t lr;
  uint64_t sp;

  uint64_t ttbr0;
};

enum task_state {
  TASK_RUNNING,
  TASK_ZOMBIE
};

#define MAX_SIGNAL_NUM 32
#define SIGKILL 9

struct task {
  struct task_context context;
  uint32_t id;
  uint32_t timeslice;
  uint32_t preempt_count;
  enum task_state state;
  int32_t exit_status;
  bool sig_pending[MAX_SIGNAL_NUM];
};

struct queue runqueue;

struct task __attribute__((aligned(16))) task_pool[MAX_TASK_NUM];
bool task_inuse[MAX_TASK_NUM];
uint8_t __attribute__((aligned(16))) kstack_pool[MAX_TASK_NUM][MAX_STACK_SIZE];

void idle_task_create(void);
uint32_t privilege_task_init(void);
uint32_t privilege_task_create(void(*func)(void));
void context_switch(struct task *next);
void schedule();

void context_switch_helper(struct task *prev, struct task *next);
struct task *get_current_task(void);
void task_debut_hook(void);

void preempt_enable(void);
void preempt_disable(void);

#endif // KERNEL_SCHED_H_
