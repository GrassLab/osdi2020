#include "schedule.h"
#include "meta_macro.h"
#include "uart.h"
#include "timer.h"
#include "irq.h"
#include "string_util.h"
#include "task.h"
#include "queue.h"

static uint64_t_queue schedule_run_queue;
int schedule_zombie_exist = 0;

void scheduler_init(void)
{
  QUEUE_INIT(schedule_run_queue);

  asm volatile("mov x0, xzr\n"
               "msr tpidr_el1, x0\n");
  task_privilege_task_create(schedule_zombie_reaper);
  /* Demo purpose */
  task_privilege_task_create(task_privilege_demo);
  task_privilege_task_create(task_user_demo);
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
  uint64_t current_privilege_task_id = task_get_current_task_id();
  if(current_privilege_task_id == 0)
  {
    /* Come from schedule_init */
    /* Call schedule_idle_task */
    asm volatile("mov x0, %0\n"
                 "msr tpidr_el1, x0\n" : : "I"(1));
    /* No need to switch from zombie_reaper to zombie_reaper */
#pragma GCC diagnostic ignored "-Wunused-value"
    QUEUE_POP(schedule_run_queue);
    /* But zombie_reaper still need to persist in run queue */
    QUEUE_PUSH(schedule_run_queue, 1);
    schedule_zombie_reaper();
  }
  else
  {
    uint64_t next_id = QUEUE_POP(schedule_run_queue);
    while(CHECK_BIT(kernel_task_pool[TASK_ID_TO_IDX(next_id)].flag, 1))
    {
      /* zomebie will not be queued */
      next_id = QUEUE_POP(schedule_run_queue);
    }
    QUEUE_PUSH(schedule_run_queue, next_id);
    schedule_context_switch(current_privilege_task_id, next_id);
  }

  return;
}

void schedule_update_quantum_count(void)
{
  uint64_t current_task_id = task_get_current_task_id();
  uint64_t current_task_idx = TASK_ID_TO_IDX(current_task_id);

  if(CHECK_BIT(kernel_task_pool[current_task_idx].flag, 0))
  {
    return;
  }

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
  uint64_t current_task_id = task_get_current_task_id();
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

void schedule_yield(void)
{
  uint64_t current_task_id = task_get_current_task_id();
  CLEAR_BIT(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].flag, 0);
  scheduler();
  return;
}

void schedule_zombie_reaper(void)
{
  char ann[0x80] = ANSI_RED"[Zombie reaper/"ANSI_GREEN"idle] "ANSI_RESET;
  uart_puts(ann);
  uart_puts("Allow me to introduce myself.\n");
  while(1)
  {
    if(schedule_zombie_exist == 0)
    {
      /* nothing to do, yield */
      schedule_yield();
      continue;
    }
    for(unsigned task_idx = 0; task_idx < TASK_POOL_SIZE; ++task_idx)
    {
      schedule_zombie_exist = 0;
      if(CHECK_BIT(kernel_task_pool[task_idx].flag, 1))
      {
        char id_in_string[0x10];

        /* free task struct */
        kernel_task_pool[task_idx].flag = 0;
        kernel_task_pool[task_idx].id = 0;

        /* remove all the zombie in schedule_run_queue */
        /* There will always be idle and zombie reaper in queue */
        int queue_length = schedule_run_queue.tail - schedule_run_queue.head;

        for(int queue_idx = 0; queue_idx < queue_length; ++queue_idx)
        {
          uint64_t task_id = QUEUE_POP(schedule_run_queue);
          /* queue back if the task is not zombie */
          if(task_id != task_idx + 1)
          {
            QUEUE_PUSH(schedule_run_queue, task_id);
          }
        }
        string_longlong_to_char(id_in_string, task_idx + 1);
        uart_puts(ann);
        uart_puts("Task id: ");
        uart_puts(id_in_string);
        uart_puts(" reaped!\n");
      }
    }
    schedule_yield();
  }
}

