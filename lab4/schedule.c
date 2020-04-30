#include "schedule.h"
#include "meta_macro.h"
#include "uart.h"
#include "timer.h"
#include "irq.h"
#include "string_util.h"
#include "queue.h"

struct task_struct kernel_task_pool[TASK_POOL_SIZE];
uint16_t kernel_stack_pool[TASK_POOL_SIZE][TASK_KERNEL_STACK_SIZE]; /* Pi Stack needs to be 16 byte aligned */
uint64_t_queue schedule_run_queue;

void scheduler_init(void)
{
  QUEUE_INIT(schedule_run_queue);

  asm volatile("mov x0, xzr\n"
               "msr tpidr_el1, x0\n");
  schedule_privilege_task_create(schedule_idle_task); /* Reserve the space for idle_task */
  /* Demo purpose */
  schedule_privilege_task_create(schedule_demo_task);
  schedule_privilege_task_create(schedule_demo_task);
  schedule_privilege_task_create(schedule_demo_task);
  /* End of demo purpose */
  irq_int_enable();
  timer_enable_core_timer();
  timer_set_core_timer_approx_ms(SCHEDULE_TIMEOUT_MS);
  scheduler();
  return;
}

void schedule_privilege_task_create(void(*start_func)())
{
  unsigned new_id = 0;

  /* find usable position in task_pool */
  for(unsigned idx = 0; idx < TASK_POOL_SIZE; ++idx)
  {
    if(kernel_task_pool[idx].id != idx + 1)
    {
      new_id = idx + 1;
      break;
    }
  }
  if(new_id == 0)
  {
    /* failed to create new task */
    return;
  }

  /* assign id */
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].id = new_id;

  /* init quantum_count to 0 */
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].quantum_count = 0;

  /* reset flag */
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].flag = 0;

  /* assign context */
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].cpu_context.lr = (uint64_t)start_func;
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].cpu_context.fp = (uint64_t)(kernel_stack_pool[TASK_ID_TO_IDX(new_id)] + TASK_KERNEL_STACK_SIZE);
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].cpu_context.sp = (uint64_t)(kernel_stack_pool[TASK_ID_TO_IDX(new_id)] + TASK_KERNEL_STACK_SIZE);

  /* push into queue */
  QUEUE_PUSH(schedule_run_queue, new_id);
  return;
}

uint64_t schedule_get_current_privilege_task_id(void)
{
  uint64_t current_task_id;
  asm volatile("mrs %0, tpidr_el1\n":
               "=r"(current_task_id));
  return current_task_id;
}

void schedule_context_switch(uint64_t current_id, uint64_t next_id)
{
  schedule_switch_context(&(kernel_task_pool[TASK_ID_TO_IDX(current_id)].cpu_context), &(kernel_task_pool[TASK_ID_TO_IDX(next_id)].cpu_context), next_id);
  return;
}

void scheduler(void)
{
  uint64_t current_privilege_task_id = schedule_get_current_privilege_task_id();
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
    schedule_idle_task();
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

/* TODO: Set brk on this function and see why it's not triggered */
void schedule_idle_task(void)
{
  while(1)
  {
    uart_puts("Hi I'm IDLE task\n");
    while(QUEUE_EMPTY(schedule_run_queue))
    {
      asm volatile ("wfi");
      /* keep waiting for new task even wake from timer interrupt */
    }
    scheduler();
  }
}

void schedule_demo_task(void)
{
  while(1)
  {
    char string_buff[0x10];
    uint64_t current_task_id = schedule_get_current_privilege_task_id();
    uart_puts("Hi I'm privilege task id ");
    string_longlong_to_char(string_buff, (int64_t)current_task_id);
    uart_puts(string_buff);
    uart_putc('\n');

    uart_puts("Waiting to reschedule\n");
    while(!schedule_check_self_reschedule())
    {
      asm volatile("wfi");
    }

    uart_puts("Time to reschedule\n");
    CLEAR_BIT(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].flag, 0);
    scheduler();
  }
}

void schedule_update_quantum_count(void)
{
  uint64_t current_task_id = schedule_get_current_privilege_task_id();
  uint64_t current_task_idx = TASK_ID_TO_IDX(current_task_id);

  if(kernel_task_pool[current_task_idx].quantum_count + 1 >= TASK_DEFAULT_QUANTUM)
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
  uint64_t current_task_id = schedule_get_current_privilege_task_id();
  return CHECK_BIT(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].flag, 0);
}

