#include "schedule.h"
#include "meta_macro.h"
#include "uart.h"
#include "timer.h"
#include "irq.h"
#include "string_util.h"
#include "task.h"
#include "queue.h"

static uint64_t_queue schedule_run_queue;

void scheduler_init(void)
{
  QUEUE_INIT(schedule_run_queue);

  asm volatile("mov x0, xzr\n"
               "msr tpidr_el1, x0\n");
  task_privilege_task_create(task_idle); /* Reserve the space for idle_task */
  /* Demo purpose */
  task_privilege_task_create(task_privilege_demo);
  task_privilege_task_create(task_privilege_demo);
  task_privilege_task_create(task_privilege_demo);
  /* End of demo purpose */
  irq_int_enable();
  timer_enable_core_timer();
  timer_set_core_timer_approx_ms(SCHEDULE_TIMEOUT_MS);
  scheduler();
  return;
}

void schedule_context_switch(uint64_t current_id, uint64_t next_id)
{
  schedule_switch_context(&(kernel_task_pool[TASK_ID_TO_IDX(current_id)].cpu_context), &(kernel_task_pool[TASK_ID_TO_IDX(next_id)].cpu_context), next_id);
  return;
}

void scheduler(void)
{
  uint64_t current_privilege_task_id = task_get_current_privilege_task_id();
  if(current_privilege_task_id == 0)
  {
    /* Come from schedule_init */
    /* Call schedule_idle_task */
    asm volatile("mov x0, %0\n"
                 "msr tpidr_el1, x0\n" : : "I"(1));
    /* No need to switch from idle_task to idle_task */
#pragma GCC diagnostic ignored "-Wunused-value"
    QUEUE_POP(schedule_run_queue);
    /* But idle_task still need to persist in run queue */
    QUEUE_PUSH(schedule_run_queue, 1);
    task_idle();
  }
  else
  {
    static int schedule_demo_task_count = 0;
    uint64_t next_id = QUEUE_POP(schedule_run_queue);
#define TOTAL_SCHEDULE_DEMO_TASK_COUNT 4
    if(schedule_demo_task_count < TOTAL_SCHEDULE_DEMO_TASK_COUNT)
    {
      QUEUE_PUSH(schedule_run_queue, next_id);
      ++schedule_demo_task_count;
    }
    schedule_context_switch(current_privilege_task_id, next_id);
  }

  return;
}

void schedule_update_quantum_count(void)
{
  uint64_t current_task_id = task_get_current_privilege_task_id();
  uint64_t current_task_idx = TASK_ID_TO_IDX(current_task_id);

  if(kernel_task_pool[current_task_idx].quantum_count + 1 >= SCHEDULE_DEFAULT_QUANTUM)
  {
    /* reset quantum count and time up */
    kernel_task_pool[current_task_idx].quantum_count = 0;
    SET_BIT(kernel_task_pool[current_task_idx].flag, 0);
  }
  else
  {
    ++kernel_task_pool[current_task_idx].quantum_count;
  }
}

int schedule_check_self_reschedule(void)
{
  uint64_t current_task_id = task_get_current_privilege_task_id();
  return CHECK_BIT(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].flag, 0);
}

void schedule_enqueue(uint64_t id)
{
  QUEUE_PUSH(schedule_run_queue, (unsigned)id);
  return;
}

int schedule_check_queue_empty(void)
{
  return QUEUE_EMPTY(schedule_run_queue);
}

