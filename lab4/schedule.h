#include <stdint.h>
#include "queue.h"
#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#define TASK_POOL_SIZE 64
#define TASK_KERNEL_STACK_SIZE 0x800
#define TASK_DEFAULT_QUANTUM 3
#define SCHEDULE_TIMEOUT_MS 10

#define TASK_ID_TO_IDX(x) ((x - 1))

struct cpu_context_struct
{
  /* callee-saved registers */
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
  uint64_t fp; /* x29 */
  uint64_t lr; /* x30 */
  uint64_t sp;
};

struct task_struct
{
  uint64_t id;
  uint64_t quantum_count;
  /* bit */
  /* bit 0 -> reschedule */
  uint64_t flag;
  struct cpu_context_struct cpu_context;
};

extern struct task_struct kernel_task_pool[TASK_POOL_SIZE];
extern uint16_t kernel_stack_pool[TASK_POOL_SIZE][TASK_KERNEL_STACK_SIZE]; /* Pi Stack needs to be 16 byte aligned */
extern uint64_t_queue schedule_run_queue;

void scheduler_init(void);
void schedule_context_switch(uint64_t current_id, uint64_t next_id);
void scheduler(void);
void schedule_update_quantum_count(void);
int schedule_check_self_reschedule(void);

extern void schedule_switch_context(struct cpu_context_struct * current_id_context_struct, struct cpu_context_struct * next_id_context_struct, uint64_t next_id);

#endif

