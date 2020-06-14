#include <stdint.h>
#include "queue.h"
#include "task.h"
#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#define SCHEDULE_DEFAULT_QUANTUM 3
#define SCHEDULE_TIMEOUT_MS 10

extern struct task_struct * kernel_task_pool;
extern uint16_t * task_kernel_stack_pool;

int scheduler_is_init(void);
void scheduler_init(void);
void schedule_context_switch(uint64_t current_id, uint64_t next_id);
void scheduler(void);
void schedule_update_quantum_count(void);
int schedule_check_self_reschedule(void);
void schedule_enqueue(uint64_t id, unsigned priority);
void schedule_enqueue_wait(uint64_t id);
uint64_t schedule_dequeue_wait(void);
int schedule_check_queue_empty(unsigned priority);
void schedule_yield(void);
void schedule_zombie_reaper(void);
void schedule_idle(void);

extern void schedule_switch_context(struct cpu_context_struct * current_id_context_struct, struct cpu_context_struct * next_id_context_struct, uint64_t next_id);

#endif

