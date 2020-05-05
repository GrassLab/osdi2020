#include "schedule.h"
#include "meta_macro.h"
#include "uart.h"
#include "timer.h"
#include "irq.h"
#include "string_util.h"
#include "task.h"
#include "queue.h"

static uint64_t_queue schedule_run_queue;
static uint64_t_queue schedule_wait_queue;
int schedule_zombie_exist = 0;

void scheduler_init(void)
{
  QUEUE_INIT(schedule_run_queue);
  QUEUE_INIT(schedule_wait_queue);

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
    /* No need to guard, there is no other task */
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
    task_guard_section();
    uint64_t next_id = QUEUE_POP(schedule_run_queue);
    uint64_t next_idx = TASK_ID_TO_IDX(next_id);
    /* task in zombie or wait state will not be queued */
    while(CHECK_BIT(kernel_task_pool[next_idx].flag, TASK_STATE_ZOMBIE) || CHECK_BIT(kernel_task_pool[next_idx].flag, TASK_STATE_WAIT))
    {
      next_id = QUEUE_POP(schedule_run_queue);
      next_idx = TASK_ID_TO_IDX(next_id);
    }
    QUEUE_PUSH(schedule_run_queue, next_id);
    schedule_context_switch(current_privilege_task_id, next_id);
    task_unguard_section();
  }

  return;
}

void schedule_update_quantum_count(void)
{
  uint64_t current_task_id = task_get_current_task_id();
  uint64_t current_task_idx = TASK_ID_TO_IDX(current_task_id);

  task_guard_section();
  if(CHECK_BIT(kernel_task_pool[current_task_idx].flag, TASK_STATE_RESCHEDULE))
  {
    task_unguard_section();
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
  task_unguard_section();
}

int schedule_check_self_reschedule(void)
{
  uint64_t current_task_id = task_get_current_task_id();
  return CHECK_BIT(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].flag, TASK_STATE_RESCHEDULE);
}

void schedule_enqueue(uint64_t id)
{
  task_guard_section();
  QUEUE_PUSH(schedule_run_queue, id);
  task_unguard_section();
  return;
}

void schedule_enqueue_wait(uint64_t id)
{
  task_guard_section();
  QUEUE_PUSH(schedule_wait_queue, id);
  task_unguard_section();
}

uint64_t schedule_dequeue_wait(void)
{
  task_guard_section();
  uint64_t id = QUEUE_POP(schedule_wait_queue);
  task_unguard_section();
  return id;
}

int schedule_check_queue_empty(void)
{
  return QUEUE_EMPTY(schedule_run_queue);
}

void schedule_yield(void)
{
  uint64_t current_task_id = task_get_current_task_id();
  task_guard_section();
  CLEAR_BIT(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].flag, 0);
  task_unguard_section();
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
      irq_int_enable();
      schedule_yield();
      continue;
    }
    task_guard_section();
    for(unsigned task_idx = 0; task_idx < TASK_POOL_SIZE; ++task_idx)
    {
      schedule_zombie_exist = 0;
      if(CHECK_BIT(kernel_task_pool[task_idx].flag, TASK_STATE_ZOMBIE))
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
    task_unguard_section();
    schedule_yield();
  }
}

