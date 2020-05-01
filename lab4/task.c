#include <stdint.h>
#include "task.h"
#include "queue.h"
#include "schedule.h"
#include "uart.h"
#include "string_util.h"

struct task_struct kernel_task_pool[TASK_POOL_SIZE];
uint16_t task_kernel_stack_pool[TASK_POOL_SIZE][TASK_KERNEL_STACK_SIZE];
uint16_t task_user_stack_pool[TASK_POOL_SIZE][TASK_USER_STACK_SIZE];

void task_privilege_task_create(void(*start_func)())
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
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].cpu_context.fp = (uint64_t)(task_kernel_stack_pool[TASK_ID_TO_IDX(new_id)] + TASK_KERNEL_STACK_SIZE);
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].cpu_context.sp = (uint64_t)(task_kernel_stack_pool[TASK_ID_TO_IDX(new_id)] + TASK_KERNEL_STACK_SIZE);

  /* push into queue */
  schedule_enqueue(new_id);
  return;
}

uint64_t task_get_current_privilege_task_id(void)
{
  uint64_t current_task_id;
  asm volatile("mrs %0, tpidr_el1\n":
               "=r"(current_task_id));
  return current_task_id;
}

void task_idle(void)
{
  while(1)
  {
    uart_puts("Hi I'm IDLE task\n");
    while(schedule_check_queue_empty())
    {
      asm volatile ("wfi");
      /* keep waiting for new task even wake from timer interrupt */
    }
    scheduler();
  }
}

void task_privilege_demo(void)
{
  while(1)
  {
    char string_buff[0x10];
    uint64_t current_task_id = task_get_current_privilege_task_id();
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



